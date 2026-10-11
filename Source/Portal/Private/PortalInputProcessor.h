// Copyright Mippithedork 2026, Inc. All Rights Reserved.

#pragma once

#include "Framework/Application/IInputProcessor.h"

class UEdGraph;

class FPortalInputProcessor final : public IInputProcessor
{
public:
    virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {}
    virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
    virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
    virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override;
    virtual const TCHAR* GetDebugName() const override { return TEXT("PortalInputProcessor"); }

    static bool GetLastGraphRightClick(const UEdGraph*& OutGraph, FVector2f& OutGraphPosition);
    static bool GetLastInteractedGraph(const UEdGraph*& OutGraph, FVector2f& OutGraphPosition);

private:
    bool bRKeyDown = false;
};
