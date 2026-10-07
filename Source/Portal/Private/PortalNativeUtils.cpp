// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalNativeUtils.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "K2Node_Knot.h"
#include "K2Node_PortalDeclaration.h"
#include "K2Node_PortalUsage.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "UObject/MetaData.h"
#include "UObject/Package.h"
#include "UObject/UObjectIterator.h"

namespace PortalNativeUtils
{
    namespace
    {
        const FName RoleKey(TEXT("Portal.Role"));
        const FName FamilyKey(TEXT("Portal.Family"));
        const FName GraphKey(TEXT("Portal.SourceGraph"));
        const FName NameKey(TEXT("Portal.Name"));
        const FName ColorKey(TEXT("Portal.Color"));

        const TCHAR* InputPrefix = TEXT("Portal Input: ");
        const TCHAR* OutputPrefix = TEXT("Portal Output: ");
        const TCHAR* LegacyDeclarationPrefix = TEXT("Portal Declaration: ");
        const TCHAR* LegacyUsagePrefix = TEXT("Portal Usage: ");

        FString RoleToString(EPortalKnotRole Role)
        {
            switch (Role)
            {
                case EPortalKnotRole::Declaration: return TEXT("Declaration");
                case EPortalKnotRole::Usage: return TEXT("Usage");
                default: return TEXT("None");
            }
        }

        EPortalKnotRole StringToRole(const FString& Value)
        {
            if (Value.Equals(TEXT("Declaration"), ESearchCase::IgnoreCase))
            {
                return EPortalKnotRole::Declaration;
            }
            if (Value.Equals(TEXT("Usage"), ESearchCase::IgnoreCase))
            {
                return EPortalKnotRole::Usage;
            }
            return EPortalKnotRole::None;
        }

        FMetaData* GetMetadata(const UObject* Object)
        {
#if WITH_METADATA
            if (Object && Object->GetPackage())
            {
                return &Object->GetPackage()->GetMetaData();
            }
#endif
            return nullptr;
        }

        UK2Node_Knot* CreateKnot(UEdGraph* Graph, const FVector2f& Position)
        {
            if (!Graph)
            {
                return nullptr;
            }

            FGraphNodeCreator<UK2Node_Knot> Creator(*Graph);
            UK2Node_Knot* Knot = Creator.CreateUserInvokedNode(false, UK2Node_Knot::StaticClass());
            if (!Knot)
            {
                return nullptr;
            }

            Knot->NodePosX = FMath::RoundToInt(Position.X);
            Knot->NodePosY = FMath::RoundToInt(Position.Y);
            Creator.Finalize();
            return Knot;
        }

        FName MakeUniqueName(const UEdGraph* Graph, FName Desired, const UK2Node_Knot* Ignore)
        {
            FString Base = Desired.IsNone() ? TEXT("Portal") : Desired.ToString().TrimStartAndEnd();
            if (Base.IsEmpty())
            {
                Base = TEXT("Portal");
            }

            FString Candidate = Base;
            int32 Suffix = 2;
            for (;;)
            {
                bool bTaken = false;
                if (Graph)
                {
                    for (UEdGraphNode* Node : Graph->Nodes)
                    {
                        const UK2Node_Knot* Knot = Cast<UK2Node_Knot>(Node);
                        if (!Knot || Knot == Ignore)
                        {
                            continue;
                        }

                        FPortalKnotData Other;
                        if (ReadPortalData(Knot, Other) && Other.Role == EPortalKnotRole::Declaration && Other.Name == FName(*Candidate))
                        {
                            bTaken = true;
                            break;
                        }
                    }
                }

                if (!bTaken)
                {
                    return FName(*Candidate);
                }

                Candidate = FString::Printf(TEXT("%s_%d"), *Base, Suffix++);
            }
        }

        bool IsExecKnot(const UK2Node_Knot* Knot)
        {
            if (!Knot)
            {
                return false;
            }

            const UEdGraphPin* Input = Knot->GetInputPin();
            const UEdGraphPin* Output = Knot->GetOutputPin();
            return (Input && Input->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec) ||
                   (Output && Output->PinType.PinCategory == UEdGraphSchema_K2::PC_Exec);
        }

        void CopyExternalLinks(UEdGraphPin* OldPin, UEdGraphPin* NewPin, const TSet<const UEdGraphNode*>& NodesToIgnore)
        {
            if (!OldPin || !NewPin)
            {
                return;
            }

            const UEdGraphSchema* Schema = NewPin->GetOwningNode() ? NewPin->GetOwningNode()->GetSchema() : nullptr;
            if (!Schema)
            {
                return;
            }

            const TArray<UEdGraphPin*> OldLinks = OldPin->LinkedTo;
            for (UEdGraphPin* Linked : OldLinks)
            {
                if (!Linked || NodesToIgnore.Contains(Linked->GetOwningNode()))
                {
                    continue;
                }
                Schema->TryCreateConnection(NewPin, Linked);
            }
        }

        bool IsPortalFallbackComment(const UK2Node_Knot* Knot)
        {
            if (!Knot)
            {
                return false;
            }
            EPortalKnotRole Role = EPortalKnotRole::None;
            FName Name;
            return ParseFallbackComment(Knot->NodeComment, Role, Name);
        }
    }

    FString MakeFallbackComment(EPortalKnotRole Role, FName Name)
    {
        const FString CleanName = Name.IsNone() ? TEXT("Portal") : Name.ToString();
        return FString(Role == EPortalKnotRole::Usage ? OutputPrefix : InputPrefix) + CleanName;
    }

    bool ParseFallbackComment(const FString& Comment, EPortalKnotRole& OutRole, FName& OutName)
    {
        OutRole = EPortalKnotRole::None;
        OutName = NAME_None;

        if (Comment.StartsWith(InputPrefix, ESearchCase::CaseSensitive))
        {
            OutRole = EPortalKnotRole::Declaration;
            OutName = FName(*Comment.RightChop(FCString::Strlen(InputPrefix)).TrimStartAndEnd());
            return true;
        }

        if (Comment.StartsWith(OutputPrefix, ESearchCase::CaseSensitive))
        {
            OutRole = EPortalKnotRole::Usage;
            OutName = FName(*Comment.RightChop(FCString::Strlen(OutputPrefix)).TrimStartAndEnd());
            return true;
        }

        // Backward compatibility with 0.4.x fallback comments.
        if (Comment.StartsWith(LegacyDeclarationPrefix, ESearchCase::CaseSensitive))
        {
            OutRole = EPortalKnotRole::Declaration;
            OutName = FName(*Comment.RightChop(FCString::Strlen(LegacyDeclarationPrefix)).TrimStartAndEnd());
            return true;
        }

        if (Comment.StartsWith(LegacyUsagePrefix, ESearchCase::CaseSensitive))
        {
            OutRole = EPortalKnotRole::Usage;
            OutName = FName(*Comment.RightChop(FCString::Strlen(LegacyUsagePrefix)).TrimStartAndEnd());
            return true;
        }

        return false;
    }

    bool ReadPortalData(const UK2Node_Knot* Knot, FPortalKnotData& OutData)
    {
        OutData = FPortalKnotData();
        if (!Knot)
        {
            return false;
        }

        bool bFoundMetadata = false;
#if WITH_METADATA
        if (FMetaData* Meta = GetMetadata(Knot))
        {
            if (const FString* RoleValue = Meta->FindValue(Knot, RoleKey))
            {
                OutData.Role = StringToRole(*RoleValue);
                bFoundMetadata = OutData.Role != EPortalKnotRole::None;
            }

            if (const FString* FamilyValue = Meta->FindValue(Knot, FamilyKey))
            {
                FGuid::Parse(*FamilyValue, OutData.FamilyGuid);
            }
            if (const FString* GraphValue = Meta->FindValue(Knot, GraphKey))
            {
                FGuid::Parse(*GraphValue, OutData.SourceGraphGuid);
            }
            if (const FString* NameValue = Meta->FindValue(Knot, NameKey))
            {
                OutData.Name = FName(**NameValue);
            }
            if (const FString* ColorValue = Meta->FindValue(Knot, ColorKey))
            {
                OutData.Color.InitFromString(*ColorValue);
            }
        }
#endif

        EPortalKnotRole CommentRole = EPortalKnotRole::None;
        FName CommentName;
        const bool bHasFallback = ParseFallbackComment(Knot->NodeComment, CommentRole, CommentName);
        if (!bFoundMetadata && bHasFallback)
        {
            OutData.Role = CommentRole;
            OutData.Name = CommentName;
            return true;
        }

        if (bFoundMetadata)
        {
            if (OutData.Name.IsNone() && bHasFallback)
            {
                OutData.Name = CommentName;
            }
            if (OutData.Name.IsNone())
            {
                OutData.Name = FName(TEXT("Portal"));
            }
            return true;
        }

        return false;
    }

    bool IsPortalKnot(const UK2Node_Knot* Knot)
    {
        FPortalKnotData Data;
        return ReadPortalData(Knot, Data) && Data.Role != EPortalKnotRole::None;
    }

    EPortalKnotRole GetRole(const UK2Node_Knot* Knot)
    {
        FPortalKnotData Data;
        return ReadPortalData(Knot, Data) ? Data.Role : EPortalKnotRole::None;
    }

    FName GetPortalName(const UK2Node_Knot* Knot)
    {
        FPortalKnotData Data;
        return ReadPortalData(Knot, Data) ? Data.Name : NAME_None;
    }

    FLinearColor GetPortalColor(const UK2Node_Knot* Knot)
    {
        FPortalKnotData Data;
        return ReadPortalData(Knot, Data) ? Data.Color : FLinearColor(0.08f, 0.42f, 0.75f, 1.0f);
    }

    void WritePortalData(UK2Node_Knot* Knot, const FPortalKnotData& InData, bool bMarkDirty)
    {
        if (!Knot || InData.Role == EPortalKnotRole::None)
        {
            return;
        }

        FPortalKnotData Data = InData;
        if (Data.Name.IsNone())
        {
            Data.Name = FName(TEXT("Portal"));
        }
        if (!Data.SourceGraphGuid.IsValid() && Knot->GetGraph())
        {
            Data.SourceGraphGuid = Knot->GetGraph()->GraphGuid;
        }

        Knot->Modify();
        Knot->NodeComment = MakeFallbackComment(Data.Role, Data.Name);

#if WITH_METADATA
        if (FMetaData* Meta = GetMetadata(Knot))
        {
            Meta->SetValue(Knot, RoleKey, *RoleToString(Data.Role));
            Meta->SetValue(Knot, FamilyKey, *Data.FamilyGuid.ToString());
            Meta->SetValue(Knot, GraphKey, *Data.SourceGraphGuid.ToString());
            Meta->SetValue(Knot, NameKey, *Data.Name.ToString());
            Meta->SetValue(Knot, ColorKey, *Data.Color.ToString());
        }
#endif

        if (bMarkDirty)
        {
            if (UPackage* Package = Knot->GetPackage())
            {
                Package->MarkPackageDirty();
            }
        }
    }

    void ClearPortalData(UK2Node_Knot* Knot, bool bKeepReadableComment)
    {
        if (!Knot)
        {
            return;
        }

        FPortalKnotData Existing;
        ReadPortalData(Knot, Existing);
        Knot->Modify();
        Knot->NodeComment = bKeepReadableComment && !Existing.Name.IsNone() ? Existing.Name.ToString() : FString();

#if WITH_METADATA
        if (FMetaData* Meta = GetMetadata(Knot))
        {
            Meta->RemoveValue(Knot, RoleKey);
            Meta->RemoveValue(Knot, FamilyKey);
            Meta->RemoveValue(Knot, GraphKey);
            Meta->RemoveValue(Knot, NameKey);
            Meta->RemoveValue(Knot, ColorKey);
        }
#endif
        if (UPackage* Package = Knot->GetPackage())
        {
            Package->MarkPackageDirty();
        }
    }

    UK2Node_Knot* FindDeclaration(const UEdGraph* Graph, const FGuid& FamilyGuid)
    {
        if (!Graph || !FamilyGuid.IsValid())
        {
            return nullptr;
        }

        for (UEdGraphNode* Node : Graph->Nodes)
        {
            UK2Node_Knot* Knot = Cast<UK2Node_Knot>(Node);
            FPortalKnotData Data;
            if (Knot && ReadPortalData(Knot, Data) && Data.Role == EPortalKnotRole::Declaration && Data.FamilyGuid == FamilyGuid)
            {
                return Knot;
            }
        }
        return nullptr;
    }

    UK2Node_Knot* FindDeclarationForUsage(const UK2Node_Knot* Usage, bool bAllowRepair)
    {
        if (!Usage || GetRole(Usage) != EPortalKnotRole::Usage || !Usage->GetGraph())
        {
            return nullptr;
        }

        if (const UEdGraphPin* Input = Usage->GetInputPin())
        {
            for (UEdGraphPin* Linked : Input->LinkedTo)
            {
                UK2Node_Knot* Candidate = Linked ? Cast<UK2Node_Knot>(Linked->GetOwningNode()) : nullptr;
                if (Candidate && GetRole(Candidate) == EPortalKnotRole::Declaration)
                {
                    return Candidate;
                }
            }
        }

        FPortalKnotData UsageData;
        if (!ReadPortalData(Usage, UsageData) || !UsageData.FamilyGuid.IsValid())
        {
            return nullptr;
        }

        UK2Node_Knot* Declaration = FindDeclaration(Usage->GetGraph(), UsageData.FamilyGuid);
        const bool bSameOriginalGraph = !UsageData.SourceGraphGuid.IsValid() || UsageData.SourceGraphGuid == Usage->GetGraph()->GraphGuid;
        if (Declaration && bAllowRepair && bSameOriginalGraph)
        {
            UK2Node_Knot* MutableUsage = const_cast<UK2Node_Knot*>(Usage);
            const UEdGraphSchema* Schema = MutableUsage->GetSchema();
            if (Schema)
            {
                MutableUsage->Modify();
                Declaration->Modify();
                MutableUsage->GetInputPin()->BreakAllPinLinks();
                Schema->TryCreateConnection(Declaration->GetOutputPin(), MutableUsage->GetInputPin());
            }
        }

        return bSameOriginalGraph ? Declaration : nullptr;
    }

    void GetUsages(const UK2Node_Knot* Declaration, TArray<UK2Node_Knot*>& OutUsages)
    {
        OutUsages.Reset();
        if (!Declaration || GetRole(Declaration) != EPortalKnotRole::Declaration || !Declaration->GetGraph())
        {
            return;
        }

        FPortalKnotData DeclarationData;
        ReadPortalData(Declaration, DeclarationData);

        if (const UEdGraphPin* Output = Declaration->GetOutputPin())
        {
            for (UEdGraphPin* Linked : Output->LinkedTo)
            {
                UK2Node_Knot* Usage = Linked ? Cast<UK2Node_Knot>(Linked->GetOwningNode()) : nullptr;
                if (Usage && GetRole(Usage) == EPortalKnotRole::Usage)
                {
                    OutUsages.AddUnique(Usage);
                }
            }
        }

        if (DeclarationData.FamilyGuid.IsValid())
        {
            for (UEdGraphNode* Node : Declaration->GetGraph()->Nodes)
            {
                UK2Node_Knot* Usage = Cast<UK2Node_Knot>(Node);
                FPortalKnotData UsageData;
                if (Usage && ReadPortalData(Usage, UsageData) && UsageData.Role == EPortalKnotRole::Usage && UsageData.FamilyGuid == DeclarationData.FamilyGuid)
                {
                    OutUsages.AddUnique(Usage);
                }
            }
        }
    }

    bool IsOrphanUsage(const UK2Node_Knot* Usage)
    {
        return GetRole(Usage) == EPortalKnotRole::Usage && FindDeclarationForUsage(Usage, false) == nullptr;
    }

    void SetPortalName(UK2Node_Knot* Declaration, const FName& RequestedName)
    {
        if (!Declaration || GetRole(Declaration) != EPortalKnotRole::Declaration)
        {
            return;
        }

        FPortalKnotData Data;
        if (!ReadPortalData(Declaration, Data))
        {
            return;
        }

        const FName UniqueName = MakeUniqueName(Declaration->GetGraph(), RequestedName, Declaration);
        Data.Name = UniqueName;
        WritePortalData(Declaration, Data, true);

        TArray<UK2Node_Knot*> Usages;
        GetUsages(Declaration, Usages);
        for (UK2Node_Knot* Usage : Usages)
        {
            FPortalKnotData UsageData;
            if (ReadPortalData(Usage, UsageData))
            {
                UsageData.Name = UniqueName;
                UsageData.Color = Data.Color;
                UsageData.FamilyGuid = Data.FamilyGuid;
                UsageData.SourceGraphGuid = Declaration->GetGraph() ? Declaration->GetGraph()->GraphGuid : FGuid();
                WritePortalData(Usage, UsageData, true);
            }
        }

        if (UEdGraph* Graph = Declaration->GetGraph())
        {
            Graph->NotifyGraphChanged();
            MarkBlueprintModified(Graph, false);
        }
    }

    UK2Node_Knot* CreatePortalInput(UEdGraph* Graph, const FVector2f& Position)
    {
        if (!Graph || !Graph->GetSchema() || !Graph->GetSchema()->IsA<UEdGraphSchema_K2>())
        {
            return nullptr;
        }

        const FScopedTransaction Transaction(NSLOCTEXT("Portal", "CreatePortalInput", "Create Portal Input"));
        Graph->Modify();

        UK2Node_Knot* Input = CreateKnot(Graph, Position);
        if (!Input)
        {
            return nullptr;
        }

        FPortalKnotData Data;
        Data.Role = EPortalKnotRole::Declaration;
        Data.FamilyGuid = FGuid::NewGuid();
        Data.SourceGraphGuid = Graph->GraphGuid;
        Data.Name = MakeUniqueName(Graph, FName(TEXT("Portal")), Input);
        Data.Color = FLinearColor(0.08f, 0.42f, 0.75f, 1.0f);
        WritePortalData(Input, Data, true);

        Graph->NotifyGraphChanged();
        MarkBlueprintModified(Graph, true);
        return Input;
    }

    bool CanCreatePortalFromOutputPin(const UEdGraphPin* SourcePin)
    {
        if (!SourcePin || SourcePin->Direction != EGPD_Output || !SourcePin->GetOwningNode())
        {
            return false;
        }

        const UEdGraph* Graph = SourcePin->GetOwningNode()->GetGraph();
        if (!Graph || !Graph->GetSchema() || !Graph->GetSchema()->IsA<UEdGraphSchema_K2>())
        {
            return false;
        }

        return SourcePin->PinType.PinCategory != UEdGraphSchema_K2::PC_Exec;
    }

    UK2Node_Knot* CreatePortalFromOutputPin(UEdGraphPin* SourcePin)
    {
        if (!CanCreatePortalFromOutputPin(SourcePin))
        {
            return nullptr;
        }

        UEdGraphNode* SourceNode = SourcePin->GetOwningNode();
        UEdGraph* Graph = SourceNode ? SourceNode->GetGraph() : nullptr;
        if (!Graph)
        {
            return nullptr;
        }

        const FScopedTransaction Transaction(NSLOCTEXT("Portal", "CreatePortalFromPin", "Create Portal"));
        Graph->Modify();
        SourceNode->Modify();

        const FVector2f Position(
            static_cast<float>(SourceNode->NodePosX + 260),
            static_cast<float>(SourceNode->NodePosY));

        UK2Node_Knot* Input = CreateKnot(Graph, Position);
        if (!Input)
        {
            return nullptr;
        }

        FString SuggestedName;
        if (!SourcePin->PinFriendlyName.IsEmpty())
        {
            SuggestedName = SourcePin->PinFriendlyName.ToString().TrimStartAndEnd();
        }
        if (SuggestedName.IsEmpty())
        {
            SuggestedName = SourcePin->PinName.ToString().TrimStartAndEnd();
        }

        const bool bGenericReturnName = SuggestedName.Equals(TEXT("Return Value"), ESearchCase::IgnoreCase) ||
            SuggestedName.Equals(TEXT("ReturnValue"), ESearchCase::IgnoreCase) ||
            SuggestedName.Equals(TEXT("Result"), ESearchCase::IgnoreCase);
        if (SuggestedName.IsEmpty() || bGenericReturnName)
        {
            SuggestedName = SourceNode->GetNodeTitle(ENodeTitleType::ListView).ToString().TrimStartAndEnd();
            SuggestedName.RemoveFromStart(TEXT("Get "));
        }
        if (SuggestedName.IsEmpty())
        {
            SuggestedName = TEXT("Portal");
        }

        FPortalKnotData Data;
        Data.Role = EPortalKnotRole::Declaration;
        Data.FamilyGuid = FGuid::NewGuid();
        Data.SourceGraphGuid = Graph->GraphGuid;
        Data.Name = MakeUniqueName(Graph, FName(*SuggestedName), Input);
        Data.Color = FLinearColor(0.08f, 0.42f, 0.75f, 1.0f);
        WritePortalData(Input, Data, true);

        if (const UEdGraphSchema* Schema = Graph->GetSchema())
        {
            if (!Schema->TryCreateConnection(SourcePin, Input->GetInputPin()))
            {
                ClearPortalData(Input, false);
                Graph->RemoveNode(Input);
                Graph->NotifyGraphChanged();
                return nullptr;
            }
        }

        Graph->NotifyGraphChanged();
        MarkBlueprintModified(Graph, true);
        return Input;
    }

    UK2Node_Knot* CreateUsage(UK2Node_Knot* Declaration, const FVector2f* OptionalPosition)
    {
        if (!Declaration || GetRole(Declaration) != EPortalKnotRole::Declaration || !Declaration->GetGraph() || IsExecKnot(Declaration))
        {
            return nullptr;
        }

        FPortalKnotData DeclarationData;
        if (!ReadPortalData(Declaration, DeclarationData))
        {
            return nullptr;
        }

        UEdGraph* Graph = Declaration->GetGraph();
        const FVector2f Position = OptionalPosition ? *OptionalPosition : FVector2f(Declaration->NodePosX + 240.0f, Declaration->NodePosY + 72.0f);

        const FScopedTransaction Transaction(NSLOCTEXT("Portal", "CreateUsage", "Create Portal Output"));
        Graph->Modify();
        Declaration->Modify();

        UK2Node_Knot* Usage = CreateKnot(Graph, Position);
        if (!Usage)
        {
            return nullptr;
        }

        FPortalKnotData UsageData = DeclarationData;
        UsageData.Role = EPortalKnotRole::Usage;
        UsageData.SourceGraphGuid = Graph->GraphGuid;
        WritePortalData(Usage, UsageData, true);

        if (const UEdGraphSchema* Schema = Graph->GetSchema())
        {
            Schema->TryCreateConnection(Declaration->GetOutputPin(), Usage->GetInputPin());
        }

        Graph->NotifyGraphChanged();
        MarkBlueprintModified(Graph, true);
        return Usage;
    }

    bool ConvertRerouteToPortal(UK2Node_Knot* Knot)
    {
        if (!Knot || IsPortalKnot(Knot) || !Knot->GetGraph() || IsExecKnot(Knot))
        {
            return false;
        }

        UEdGraph* Graph = Knot->GetGraph();
        const FScopedTransaction Transaction(NSLOCTEXT("Portal", "ConvertReroute", "Convert Reroute to Portal"));
        Graph->Modify();
        Knot->Modify();

        FPortalKnotData Data;
        Data.Role = EPortalKnotRole::Declaration;
        Data.FamilyGuid = FGuid::NewGuid();
        Data.SourceGraphGuid = Graph->GraphGuid;
        Data.Name = MakeUniqueName(Graph, Knot->NodeComment.IsEmpty() ? FName(TEXT("Portal")) : FName(*Knot->NodeComment), Knot);
        Data.Color = FLinearColor(0.08f, 0.42f, 0.75f, 1.0f);

        const TArray<UEdGraphPin*> DownstreamLinks = Knot->GetOutputPin() ? Knot->GetOutputPin()->LinkedTo : TArray<UEdGraphPin*>();
        if (Knot->GetOutputPin())
        {
            Knot->GetOutputPin()->BreakAllPinLinks();
        }

        WritePortalData(Knot, Data, true);

        const UEdGraphSchema* Schema = Graph->GetSchema();
        int32 UsageIndex = 0;
        for (UEdGraphPin* TargetPin : DownstreamLinks)
        {
            if (!TargetPin || !TargetPin->GetOwningNode())
            {
                continue;
            }

            const UEdGraphNode* TargetNode = TargetPin->GetOwningNode();
            FVector2f Position(
                static_cast<float>(TargetNode->NodePosX - 180),
                static_cast<float>(TargetNode->NodePosY + UsageIndex * 36));
            UK2Node_Knot* Usage = CreateKnot(Graph, Position);
            if (!Usage)
            {
                continue;
            }

            FPortalKnotData UsageData = Data;
            UsageData.Role = EPortalKnotRole::Usage;
            WritePortalData(Usage, UsageData, true);

            if (Schema)
            {
                Schema->TryCreateConnection(Knot->GetOutputPin(), Usage->GetInputPin());
                Schema->TryCreateConnection(Usage->GetOutputPin(), TargetPin);
            }
            ++UsageIndex;
        }

        Graph->NotifyGraphChanged();
        MarkBlueprintModified(Graph, true);
        return true;
    }

    void ConvertPortalFamilyToReroutes(UK2Node_Knot* AnyFamilyNode)
    {
        if (!AnyFamilyNode || !IsPortalKnot(AnyFamilyNode))
        {
            return;
        }

        UK2Node_Knot* Declaration = GetRole(AnyFamilyNode) == EPortalKnotRole::Declaration ? AnyFamilyNode : FindDeclarationForUsage(AnyFamilyNode, false);
        if (!Declaration)
        {
            ClearPortalData(AnyFamilyNode, true);
            if (AnyFamilyNode->GetGraph())
            {
                AnyFamilyNode->GetGraph()->NotifyGraphChanged();
                MarkBlueprintModified(AnyFamilyNode->GetGraph(), false);
            }
            return;
        }

        const FScopedTransaction Transaction(NSLOCTEXT("Portal", "ConvertFamilyToReroutes", "Convert Portal to Reroutes"));
        UEdGraph* Graph = Declaration->GetGraph();
        if (Graph)
        {
            Graph->Modify();
        }

        TArray<UK2Node_Knot*> Usages;
        GetUsages(Declaration, Usages);
        ClearPortalData(Declaration, true);
        for (UK2Node_Knot* Usage : Usages)
        {
            ClearPortalData(Usage, true);
        }

        if (Graph)
        {
            Graph->NotifyGraphChanged();
            MarkBlueprintModified(Graph, false);
        }
    }

    bool MigrateLegacyGraph(UEdGraph* Graph)
    {
        if (!Graph)
        {
            return false;
        }

        TArray<UK2Node_PortalDeclaration*> LegacyDeclarations;
        TArray<UK2Node_PortalUsage*> LegacyUsages;
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            if (UK2Node_PortalDeclaration* Declaration = Cast<UK2Node_PortalDeclaration>(Node))
            {
                LegacyDeclarations.Add(Declaration);
            }
            else if (UK2Node_PortalUsage* Usage = Cast<UK2Node_PortalUsage>(Node))
            {
                LegacyUsages.Add(Usage);
            }
        }

        if (LegacyDeclarations.IsEmpty() && LegacyUsages.IsEmpty())
        {
            return false;
        }

        Graph->Modify();
        TMap<UK2Node_PortalDeclaration*, UK2Node_Knot*> DeclarationMap;
        TSet<const UEdGraphNode*> LegacyNodeSet;
        for (UK2Node_PortalDeclaration* Declaration : LegacyDeclarations)
        {
            LegacyNodeSet.Add(Declaration);
        }
        for (UK2Node_PortalUsage* Usage : LegacyUsages)
        {
            LegacyNodeSet.Add(Usage);
        }

        for (UK2Node_PortalDeclaration* Legacy : LegacyDeclarations)
        {
            UK2Node_Knot* Native = CreateKnot(Graph, FVector2f(Legacy->NodePosX, Legacy->NodePosY));
            if (!Native)
            {
                continue;
            }

            FPortalKnotData Data;
            Data.Role = EPortalKnotRole::Declaration;
            Data.FamilyGuid = Legacy->PortalGuid.IsValid() ? Legacy->PortalGuid : FGuid::NewGuid();
            Data.SourceGraphGuid = Graph->GraphGuid;
            Data.Name = Legacy->PortalName.IsNone() ? FName(TEXT("Portal")) : Legacy->PortalName;
            Data.Color = Legacy->PortalColor;
            WritePortalData(Native, Data, true);
            CopyExternalLinks(Legacy->GetInputPin(), Native->GetInputPin(), LegacyNodeSet);
            DeclarationMap.Add(Legacy, Native);
        }

        for (UK2Node_PortalUsage* Legacy : LegacyUsages)
        {
            UK2Node_PortalDeclaration* LegacyDeclaration = Legacy->Declaration.Get();
            if (!LegacyDeclaration)
            {
                LegacyDeclaration = Cast<UK2Node_PortalDeclaration>(Legacy->GetInputPin() && !Legacy->GetInputPin()->LinkedTo.IsEmpty() ? Legacy->GetInputPin()->LinkedTo[0]->GetOwningNode() : nullptr);
            }

            UK2Node_Knot* NativeDeclaration = LegacyDeclaration ? DeclarationMap.FindRef(LegacyDeclaration) : nullptr;
            UK2Node_Knot* NativeUsage = CreateKnot(Graph, FVector2f(Legacy->NodePosX, Legacy->NodePosY));
            if (!NativeUsage)
            {
                continue;
            }

            FPortalKnotData Data;
            Data.Role = EPortalKnotRole::Usage;
            Data.FamilyGuid = NativeDeclaration ? [&]() { FPortalKnotData D; ReadPortalData(NativeDeclaration, D); return D.FamilyGuid; }() : Legacy->DeclarationGuid;
            Data.SourceGraphGuid = Graph->GraphGuid;
            Data.Name = Legacy->CachedPortalName.IsNone() && LegacyDeclaration ? LegacyDeclaration->PortalName : Legacy->CachedPortalName;
            if (Data.Name.IsNone()) Data.Name = FName(TEXT("Portal"));
            Data.Color = LegacyDeclaration ? LegacyDeclaration->PortalColor : Legacy->CachedPortalColor;
            WritePortalData(NativeUsage, Data, true);

            if (NativeDeclaration && Graph->GetSchema())
            {
                Graph->GetSchema()->TryCreateConnection(NativeDeclaration->GetOutputPin(), NativeUsage->GetInputPin());
            }
            CopyExternalLinks(Legacy->GetOutputPin(), NativeUsage->GetOutputPin(), LegacyNodeSet);
        }

        for (UK2Node_PortalUsage* Legacy : LegacyUsages)
        {
            if (!Legacy) continue;
            for (UEdGraphPin* Pin : Legacy->Pins) if (Pin) Pin->BreakAllPinLinks();
            Graph->RemoveNode(Legacy);
        }
        for (UK2Node_PortalDeclaration* Legacy : LegacyDeclarations)
        {
            if (!Legacy) continue;
            for (UEdGraphPin* Pin : Legacy->Pins) if (Pin) Pin->BreakAllPinLinks();
            Graph->RemoveNode(Legacy);
        }

        Graph->NotifyGraphChanged();
        MarkBlueprintModified(Graph, true);
        return true;
    }

    bool RepairGraph(UEdGraph* Graph)
    {
        if (!Graph)
        {
            return false;
        }

        bool bChanged = false;
        TArray<UK2Node_Knot*> Knots;
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            if (UK2Node_Knot* Knot = Cast<UK2Node_Knot>(Node))
            {
                if (IsPortalKnot(Knot) || IsPortalFallbackComment(Knot))
                {
                    Knots.Add(Knot);
                }
            }
        }

        // Portal is intentionally data-only. Native UK2Node_Knot execution reroutes
        // have single-path execution semantics, so a one-input/many-output Portal family
        // would be ambiguous and can leave detached wildcard Outputs. If an old Portal or
        // a wildcard Portal becomes exec-typed, safely demote the whole family back to
        // ordinary native reroutes.
        TSet<FGuid> ExecFamilies;
        TArray<UK2Node_Knot*> ExecOrphans;
        for (UK2Node_Knot* Knot : Knots)
        {
            if (!Knot || !IsPortalKnot(Knot) || !IsExecKnot(Knot))
            {
                continue;
            }

            FPortalKnotData Data;
            if (ReadPortalData(Knot, Data) && Data.FamilyGuid.IsValid())
            {
                ExecFamilies.Add(Data.FamilyGuid);
            }
            else
            {
                ExecOrphans.Add(Knot);
            }
        }

        for (const FGuid& FamilyGuid : ExecFamilies)
        {
            UK2Node_Knot* Declaration = FindDeclaration(Graph, FamilyGuid);
            if (Declaration)
            {
                TArray<UK2Node_Knot*> FamilyUsages;
                GetUsages(Declaration, FamilyUsages);
                ClearPortalData(Declaration, true);
                for (UK2Node_Knot* Usage : FamilyUsages)
                {
                    ClearPortalData(Usage, true);
                }
                bChanged = true;
            }
            else
            {
                for (UK2Node_Knot* Knot : Knots)
                {
                    FPortalKnotData Data;
                    if (Knot && ReadPortalData(Knot, Data) && Data.FamilyGuid == FamilyGuid)
                    {
                        ClearPortalData(Knot, true);
                        bChanged = true;
                    }
                }
            }
        }

        for (UK2Node_Knot* Knot : ExecOrphans)
        {
            ClearPortalData(Knot, true);
            bChanged = true;
        }

        if (!ExecFamilies.IsEmpty() || !ExecOrphans.IsEmpty())
        {
            Knots.RemoveAll([](const UK2Node_Knot* Knot)
            {
                return !Knot || !IsPortalKnot(Knot);
            });
        }

        // First materialize metadata for nodes recovered from the native comment fallback.
        for (UK2Node_Knot* Knot : Knots)
        {
            FPortalKnotData Data;
            const bool bHasData = ReadPortalData(Knot, Data);
            if (!bHasData || Data.Role == EPortalKnotRole::None)
            {
                continue;
            }

            bool bNeedsWrite = Knot->NodeComment != MakeFallbackComment(Data.Role, Data.Name);
            if (Data.Role == EPortalKnotRole::Declaration && !Data.FamilyGuid.IsValid())
            {
                Data.FamilyGuid = FGuid::NewGuid();
                Data.SourceGraphGuid = Graph->GraphGuid;
                Data.Name = MakeUniqueName(Graph, Data.Name, Knot);
                bNeedsWrite = true;
            }
            else if (Data.Role == EPortalKnotRole::Usage && !Data.FamilyGuid.IsValid())
            {
                if (const UEdGraphPin* Input = Knot->GetInputPin())
                {
                    for (UEdGraphPin* Linked : Input->LinkedTo)
                    {
                        UK2Node_Knot* PossibleDeclaration = Linked ? Cast<UK2Node_Knot>(Linked->GetOwningNode()) : nullptr;
                        FPortalKnotData DeclarationData;
                        if (PossibleDeclaration && ReadPortalData(PossibleDeclaration, DeclarationData) && DeclarationData.Role == EPortalKnotRole::Declaration)
                        {
                            Data.FamilyGuid = DeclarationData.FamilyGuid;
                            Data.SourceGraphGuid = Graph->GraphGuid;
                            Data.Name = DeclarationData.Name;
                            Data.Color = DeclarationData.Color;
                            bNeedsWrite = true;
                            break;
                        }
                    }
                }
            }

            if (bNeedsWrite)
            {
                WritePortalData(Knot, Data, true);
                bChanged = true;
            }
        }

        // Resolve copied-family GUID collisions. The physically connected usages follow their declaration.
        TMap<FGuid, TArray<UK2Node_Knot*>> DeclarationsByFamily;
        for (UK2Node_Knot* Knot : Knots)
        {
            FPortalKnotData Data;
            if (ReadPortalData(Knot, Data) && Data.Role == EPortalKnotRole::Declaration && Data.FamilyGuid.IsValid())
            {
                DeclarationsByFamily.FindOrAdd(Data.FamilyGuid).Add(Knot);
            }
        }

        for (TPair<FGuid, TArray<UK2Node_Knot*>>& Pair : DeclarationsByFamily)
        {
            if (Pair.Value.Num() <= 1)
            {
                continue;
            }

            for (int32 Index = 1; Index < Pair.Value.Num(); ++Index)
            {
                UK2Node_Knot* DuplicateDeclaration = Pair.Value[Index];
                FPortalKnotData DeclarationData;
                ReadPortalData(DuplicateDeclaration, DeclarationData);
                DeclarationData.FamilyGuid = FGuid::NewGuid();
                DeclarationData.SourceGraphGuid = Graph->GraphGuid;
                DeclarationData.Name = MakeUniqueName(Graph, DeclarationData.Name, DuplicateDeclaration);
                WritePortalData(DuplicateDeclaration, DeclarationData, true);

                if (UEdGraphPin* Output = DuplicateDeclaration->GetOutputPin())
                {
                    for (UEdGraphPin* Linked : Output->LinkedTo)
                    {
                        UK2Node_Knot* ConnectedUsage = Linked ? Cast<UK2Node_Knot>(Linked->GetOwningNode()) : nullptr;
                        FPortalKnotData UsageData;
                        if (ConnectedUsage && ReadPortalData(ConnectedUsage, UsageData) && UsageData.Role == EPortalKnotRole::Usage)
                        {
                            UsageData.FamilyGuid = DeclarationData.FamilyGuid;
                            UsageData.SourceGraphGuid = Graph->GraphGuid;
                            UsageData.Name = DeclarationData.Name;
                            UsageData.Color = DeclarationData.Color;
                            WritePortalData(ConnectedUsage, UsageData, true);
                        }
                    }
                }
                bChanged = true;
            }
        }

        // Keep usage presentation synchronized and repair same-graph detached usages when identity is available.
        for (UK2Node_Knot* Knot : Knots)
        {
            FPortalKnotData UsageData;
            if (!ReadPortalData(Knot, UsageData) || UsageData.Role != EPortalKnotRole::Usage)
            {
                continue;
            }

            if (UK2Node_Knot* Declaration = FindDeclarationForUsage(Knot, true))
            {
                FPortalKnotData DeclarationData;
                if (ReadPortalData(Declaration, DeclarationData) &&
                    (UsageData.Name != DeclarationData.Name || UsageData.Color != DeclarationData.Color || UsageData.FamilyGuid != DeclarationData.FamilyGuid))
                {
                    UsageData.Name = DeclarationData.Name;
                    UsageData.Color = DeclarationData.Color;
                    UsageData.FamilyGuid = DeclarationData.FamilyGuid;
                    UsageData.SourceGraphGuid = Graph->GraphGuid;
                    WritePortalData(Knot, UsageData, true);
                    bChanged = true;
                }
            }
        }

        if (bChanged)
        {
            Graph->NotifyGraphChanged();
            MarkBlueprintModified(Graph, false);
        }
        return bChanged;
    }

    bool MigrateAndRepairBlueprint(UBlueprint* Blueprint)
    {
        if (!Blueprint)
        {
            return false;
        }

        bool bChanged = false;
        TSet<UEdGraph*> Graphs;

        TArray<UK2Node_Knot*> NativeKnots;
        FBlueprintEditorUtils::GetAllNodesOfClass(Blueprint, NativeKnots);
        for (UK2Node_Knot* Knot : NativeKnots)
        {
            if (Knot && Knot->GetGraph())
            {
                Graphs.Add(Knot->GetGraph());
            }
        }

        TArray<UK2Node_PortalDeclaration*> LegacyDeclarations;
        FBlueprintEditorUtils::GetAllNodesOfClass(Blueprint, LegacyDeclarations);
        for (UK2Node_PortalDeclaration* Declaration : LegacyDeclarations)
        {
            if (Declaration && Declaration->GetGraph())
            {
                Graphs.Add(Declaration->GetGraph());
            }
        }

        TArray<UK2Node_PortalUsage*> LegacyUsages;
        FBlueprintEditorUtils::GetAllNodesOfClass(Blueprint, LegacyUsages);
        for (UK2Node_PortalUsage* Usage : LegacyUsages)
        {
            if (Usage && Usage->GetGraph())
            {
                Graphs.Add(Usage->GetGraph());
            }
        }

        for (UEdGraph* Graph : Graphs)
        {
            bChanged |= MigrateLegacyGraph(Graph);
            bChanged |= RepairGraph(Graph);
        }
        return bChanged;
    }

    void MigrateAndRepairLoadedBlueprints()
    {
        for (TObjectIterator<UBlueprint> It; It; ++It)
        {
            UBlueprint* Blueprint = *It;
            if (!Blueprint || Blueprint->HasAnyFlags(RF_ClassDefaultObject | RF_Transient))
            {
                continue;
            }
            MigrateAndRepairBlueprint(Blueprint);
        }
    }

    void MarkBlueprintModified(const UEdGraph* Graph, bool bStructural)
    {
        if (!Graph)
        {
            return;
        }

        if (UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph))
        {
            Blueprint->Modify();
            if (bStructural)
            {
                FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
            }
            else
            {
                FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
            }
        }
        if (UPackage* Package = Graph->GetPackage())
        {
            Package->MarkPackageDirty();
        }
    }
}
