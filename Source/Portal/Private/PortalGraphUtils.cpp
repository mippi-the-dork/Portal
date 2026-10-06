// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#include "PortalGraphUtils.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "Engine/Blueprint.h"
#include "K2Node_PortalDeclaration.h"
#include "K2Node_PortalUsage.h"
#include "Kismet2/BlueprintEditorUtils.h"

namespace PortalGraphUtils
{
    UK2Node_PortalDeclaration* FindDeclarationByGuid(const UEdGraph* Graph, const FGuid& Guid, const UK2Node_PortalDeclaration* Ignore)
    {
        if (!Graph || !Guid.IsValid())
        {
            return nullptr;
        }

        for (UEdGraphNode* Node : Graph->Nodes)
        {
            UK2Node_PortalDeclaration* Declaration = Cast<UK2Node_PortalDeclaration>(Node);
            if (Declaration && Declaration != Ignore && Declaration->PortalGuid == Guid)
            {
                return Declaration;
            }
        }

        return nullptr;
    }

    void GetUsagesForDeclaration(const UK2Node_PortalDeclaration* Declaration, TArray<UK2Node_PortalUsage*>& OutUsages)
    {
        OutUsages.Reset();
        if (!Declaration || !Declaration->GetGraph())
        {
            return;
        }

        const FGuid Guid = Declaration->PortalGuid;

        if (const UEdGraphPin* HiddenOutput = Declaration->GetOutputPin())
        {
            for (UEdGraphPin* LinkedPin : HiddenOutput->LinkedTo)
            {
                if (LinkedPin)
                {
                    if (UK2Node_PortalUsage* LinkedUsage = Cast<UK2Node_PortalUsage>(LinkedPin->GetOwningNode()))
                    {
                        OutUsages.AddUnique(LinkedUsage);
                    }
                }
            }
        }

        for (UEdGraphNode* Node : Declaration->GetGraph()->Nodes)
        {
            UK2Node_PortalUsage* Usage = Cast<UK2Node_PortalUsage>(Node);
            if (!Usage)
            {
                continue;
            }

            const bool bPointerMatch = Usage->Declaration == Declaration;
            const bool bGuidMatch = Guid.IsValid() && Usage->DeclarationGuid == Guid;
            const bool bGraphIdentityCompatible = !Usage->DeclarationGraphGuid.IsValid() || Usage->DeclarationGraphGuid == Declaration->GetGraph()->GraphGuid;
            if (bPointerMatch || (bGuidMatch && bGraphIdentityCompatible))
            {
                OutUsages.AddUnique(Usage);
            }
        }
    }

    bool IsExternalPortalPin(const UK2Node_PortalDeclaration* Declaration, const UEdGraphPin* Pin)
    {
        return Declaration && Pin && Pin == Declaration->GetInputPin();
    }

    bool IsExternalPortalPin(const UK2Node_PortalUsage* Usage, const UEdGraphPin* Pin)
    {
        return Usage && Pin && Pin == Usage->GetOutputPin();
    }

    void MarkOwningBlueprintModified(const UEdGraph* Graph)
    {
        if (!Graph)
        {
            return;
        }

        if (UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph))
        {
            FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
        }
    }
}
