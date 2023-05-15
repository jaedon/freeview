
#ifndef __ASN1DECODE_H__
#define __ASN1DECODE_H__

// hmkim : added.
#include "../ci_ammi.h"
#ifdef _CI_DEBUG_AMMI_DECODE_FILE_DATA_

#include <stdio.h>
#include <stdbool.h>

int asn1decode_BOOLEAN(FILE *, FILE *, int);
bool is_BOOLEAN(unsigned char, unsigned int);

int asn1decode_INTEGER(FILE *, FILE *, int);
bool is_INTEGER(unsigned char, unsigned int);

int asn1decode_Null(FILE *, FILE *, int);
bool is_Null(unsigned char, unsigned int);

int asn1decode_InterchangedObject(FILE *, FILE *, int);
bool is_InterchangedObject(unsigned char, unsigned int);

int asn1decode_Root(FILE *, FILE *, int);
bool is_Root(unsigned char, unsigned int);

int asn1decode_ObjectIdentifier(FILE *, FILE *, int);
bool is_ObjectIdentifier(unsigned char, unsigned int);

int asn1decode_Group(FILE *, FILE *, int);
bool is_Group(unsigned char, unsigned int);

int asn1decode_StandardIdentifier(FILE *, FILE *, int);
bool is_StandardIdentifier(unsigned char, unsigned int);

int asn1decode_JointIsoItuIdentifier(FILE *, FILE *, int);
bool is_JointIsoItuIdentifier(unsigned char, unsigned int);

int asn1decode_MHEGStandardIdentifier(FILE *, FILE *, int);
bool is_MHEGStandardIdentifier(unsigned char, unsigned int);

int asn1decode_StandardVersion(FILE *, FILE *, int);
bool is_StandardVersion(unsigned char, unsigned int);

int asn1decode_ObjectInformation(FILE *, FILE *, int);
bool is_ObjectInformation(unsigned char, unsigned int);

int asn1decode_OnStartUp(FILE *, FILE *, int);
bool is_OnStartUp(unsigned char, unsigned int);

int asn1decode_OnCloseDown(FILE *, FILE *, int);
bool is_OnCloseDown(unsigned char, unsigned int);

int asn1decode_OriginalGroupCachePriority(FILE *, FILE *, int);
bool is_OriginalGroupCachePriority(unsigned char, unsigned int);

int asn1decode_Items(FILE *, FILE *, int);
bool is_Items(unsigned char, unsigned int);

int asn1decode_GroupItem(FILE *, FILE *, int);
bool is_GroupItem(unsigned char, unsigned int);

int asn1decode_ApplicationClass(FILE *, FILE *, int);
bool is_ApplicationClass(unsigned char, unsigned int);

int asn1decode_OnSpawnCloseDown(FILE *, FILE *, int);
bool is_OnSpawnCloseDown(unsigned char, unsigned int);

int asn1decode_OnRestart(FILE *, FILE *, int);
bool is_OnRestart(unsigned char, unsigned int);

int asn1decode_DefaultAttributes(FILE *, FILE *, int);
bool is_DefaultAttributes(unsigned char, unsigned int);

int asn1decode_DefaultAttribute(FILE *, FILE *, int);
bool is_DefaultAttribute(unsigned char, unsigned int);

int asn1decode_CharacterSet(FILE *, FILE *, int);
bool is_CharacterSet(unsigned char, unsigned int);

int asn1decode_BackgroundColour(FILE *, FILE *, int);
bool is_BackgroundColour(unsigned char, unsigned int);

int asn1decode_TextContentHook(FILE *, FILE *, int);
bool is_TextContentHook(unsigned char, unsigned int);

int asn1decode_TextColour(FILE *, FILE *, int);
bool is_TextColour(unsigned char, unsigned int);

int asn1decode_Font(FILE *, FILE *, int);
bool is_Font(unsigned char, unsigned int);

int asn1decode_FontBody(FILE *, FILE *, int);
bool is_FontBody(unsigned char, unsigned int);

int asn1decode_DirectFont(FILE *, FILE *, int);
bool is_DirectFont(unsigned char, unsigned int);

int asn1decode_IndirectFont(FILE *, FILE *, int);
bool is_IndirectFont(unsigned char, unsigned int);

int asn1decode_FontAttributes(FILE *, FILE *, int);
bool is_FontAttributes(unsigned char, unsigned int);

int asn1decode_InterchangedProgramContentHook(FILE *, FILE *, int);
bool is_InterchangedProgramContentHook(unsigned char, unsigned int);

int asn1decode_StreamContentHook(FILE *, FILE *, int);
bool is_StreamContentHook(unsigned char, unsigned int);

int asn1decode_BitmapContentHook(FILE *, FILE *, int);
bool is_BitmapContentHook(unsigned char, unsigned int);

int asn1decode_LineArtContentHook(FILE *, FILE *, int);
bool is_LineArtContentHook(unsigned char, unsigned int);

int asn1decode_ButtonRefColour(FILE *, FILE *, int);
bool is_ButtonRefColour(unsigned char, unsigned int);

int asn1decode_HighlightRefColour(FILE *, FILE *, int);
bool is_HighlightRefColour(unsigned char, unsigned int);

int asn1decode_SliderRefColour(FILE *, FILE *, int);
bool is_SliderRefColour(unsigned char, unsigned int);

int asn1decode_SceneClass(FILE *, FILE *, int);
bool is_SceneClass(unsigned char, unsigned int);

int asn1decode_InputEventRegister(FILE *, FILE *, int);
bool is_InputEventRegister(unsigned char, unsigned int);

int asn1decode_SceneCoordinateSystem(FILE *, FILE *, int);
bool is_SceneCoordinateSystem(unsigned char, unsigned int);

int asn1decode_XScene(FILE *, FILE *, int);
bool is_XScene(unsigned char, unsigned int);

int asn1decode_YScene(FILE *, FILE *, int);
bool is_YScene(unsigned char, unsigned int);

int asn1decode_AspectRatio(FILE *, FILE *, int);
bool is_AspectRatio(unsigned char, unsigned int);

int asn1decode_Width(FILE *, FILE *, int);
bool is_Width(unsigned char, unsigned int);

int asn1decode_Height(FILE *, FILE *, int);
bool is_Height(unsigned char, unsigned int);

int asn1decode_MovingCursor(FILE *, FILE *, int);
bool is_MovingCursor(unsigned char, unsigned int);

int asn1decode_NextScenes(FILE *, FILE *, int);
bool is_NextScenes(unsigned char, unsigned int);

int asn1decode_NextScene(FILE *, FILE *, int);
bool is_NextScene(unsigned char, unsigned int);

int asn1decode_InputEventMask(FILE *der, FILE *out, int length);
bool is_InputEventMask(unsigned char, unsigned int);

int asn1decode_SceneRef(FILE *, FILE *, int);
bool is_SceneRef(unsigned char, unsigned int);

int asn1decode_SceneWeight(FILE *, FILE *, int);
bool is_SceneWeight(unsigned char, unsigned int);

int asn1decode_Ingredient(FILE *, FILE *, int);
bool is_Ingredient(unsigned char, unsigned int);

int asn1decode_InitiallyActive(FILE *, FILE *, int);
bool is_InitiallyActive(unsigned char, unsigned int);

int asn1decode_ContentHook(FILE *, FILE *, int);
bool is_ContentHook(unsigned char, unsigned int);

int asn1decode_OriginalContent(FILE *, FILE *, int);
bool is_OriginalContent(unsigned char, unsigned int);

int asn1decode_ContentBody(FILE *, FILE *, int);
bool is_ContentBody(unsigned char, unsigned int);

int asn1decode_IncludedContent(FILE *, FILE *, int);
bool is_IncludedContent(unsigned char, unsigned int);

int asn1decode_ReferencedContent(FILE *, FILE *, int);
bool is_ReferencedContent(unsigned char, unsigned int);

int asn1decode_ContentSize(FILE *, FILE *, int);
bool is_ContentSize(unsigned char, unsigned int);

int asn1decode_ContentCachePriority(FILE *, FILE *, int);
bool is_ContentCachePriority(unsigned char, unsigned int);

int asn1decode_Shared(FILE *, FILE *, int);
bool is_Shared(unsigned char, unsigned int);

int asn1decode_LinkClass(FILE *, FILE *, int);
bool is_LinkClass(unsigned char, unsigned int);

int asn1decode_LinkCondition(FILE *, FILE *, int);
bool is_LinkCondition(unsigned char, unsigned int);

int asn1decode_EventSource(FILE *, FILE *, int);
bool is_EventSource(unsigned char, unsigned int);

int asn1decode_EventType(FILE *, FILE *, int);
bool is_EventType(unsigned char, unsigned int);

int asn1decode_EventTypeEnum(FILE *, FILE *, int);
bool is_EventTypeEnum(unsigned char, unsigned int);

int der_decode_EventTypeEnum(FILE *, FILE *, int);
int asn1decode_EventData(FILE *, FILE *, int);
bool is_EventData(unsigned char, unsigned int);

int asn1decode_EventDataBody(FILE *, FILE *, int);
bool is_EventDataBody(unsigned char, unsigned int);

int asn1decode_LinkEffect(FILE *, FILE *, int);
bool is_LinkEffect(unsigned char, unsigned int);

int asn1decode_Program(FILE *, FILE *, int);
bool is_Program(unsigned char, unsigned int);

int asn1decode_Name(FILE *, FILE *, int);
bool is_Name(unsigned char, unsigned int);

int asn1decode_InitiallyAvailable(FILE *, FILE *, int);
bool is_InitiallyAvailable(unsigned char, unsigned int);

int asn1decode_ResidentProgramClass(FILE *, FILE *, int);
bool is_ResidentProgramClass(unsigned char, unsigned int);

int asn1decode_RemoteProgramClass(FILE *, FILE *, int);
bool is_RemoteProgramClass(unsigned char, unsigned int);

int asn1decode_ProgramConnectionTag(FILE *, FILE *, int);
bool is_ProgramConnectionTag(unsigned char, unsigned int);

int asn1decode_InterchangedProgramClass(FILE *, FILE *, int);
bool is_InterchangedProgramClass(unsigned char, unsigned int);

int asn1decode_PaletteClass(FILE *, FILE *, int);
bool is_PaletteClass(unsigned char, unsigned int);

int asn1decode_FontClass(FILE *, FILE *, int);
bool is_FontClass(unsigned char, unsigned int);

int asn1decode_CursorShapeClass(FILE *, FILE *, int);
bool is_CursorShapeClass(unsigned char, unsigned int);

int asn1decode_Variable(FILE *, FILE *, int);
bool is_Variable(unsigned char, unsigned int);

int asn1decode_OriginalValue(FILE *, FILE *, int);
bool is_OriginalValue(unsigned char, unsigned int);

int asn1decode_OriginalValueBody(FILE *, FILE *, int);
bool is_OriginalValueBody(unsigned char, unsigned int);

int asn1decode_ObjectReferenceValue(FILE *, FILE *, int);
bool is_ObjectReferenceValue(unsigned char, unsigned int);

int asn1decode_ContentReferenceValue(FILE *, FILE *, int);
bool is_ContentReferenceValue(unsigned char, unsigned int);

int asn1decode_BooleanVariableClass(FILE *, FILE *, int);
bool is_BooleanVariableClass(unsigned char, unsigned int);

int asn1decode_IntegerVariableClass(FILE *, FILE *, int);
bool is_IntegerVariableClass(unsigned char, unsigned int);

int asn1decode_OctetStringVariableClass(FILE *, FILE *, int);
bool is_OctetStringVariableClass(unsigned char, unsigned int);

int asn1decode_ObjectRefVariableClass(FILE *, FILE *, int);
bool is_ObjectRefVariableClass(unsigned char, unsigned int);

int asn1decode_ContentRefVariableClass(FILE *, FILE *, int);
bool is_ContentRefVariableClass(unsigned char, unsigned int);

int asn1decode_Presentable(FILE *, FILE *, int);
bool is_Presentable(unsigned char, unsigned int);

int asn1decode_TokenManager(FILE *, FILE *, int);
bool is_TokenManager(unsigned char, unsigned int);

int asn1decode_MovementTable(FILE *, FILE *, int);
bool is_MovementTable(unsigned char, unsigned int);

int asn1decode_Movement(FILE *, FILE *, int);
bool is_Movement(unsigned char, unsigned int);

int asn1decode_TargetElement(FILE *, FILE *, int);
bool is_TargetElement(unsigned char, unsigned int);

int asn1decode_TokenGroupClass(FILE *, FILE *, int);
bool is_TokenGroupClass(unsigned char, unsigned int);

int asn1decode_TokenGroupBody(FILE *, FILE *, int);
bool is_TokenGroupBody(unsigned char, unsigned int);

int asn1decode_TokenGroupItems(FILE *, FILE *, int);
bool is_TokenGroupItems(unsigned char, unsigned int);

int asn1decode_TokenGroupItem(FILE *, FILE *, int);
bool is_TokenGroupItem(unsigned char, unsigned int);

int asn1decode_AVisible(FILE *, FILE *, int);
bool is_AVisible(unsigned char, unsigned int);

int asn1decode_ActionSlots(FILE *, FILE *, int);
bool is_ActionSlots(unsigned char, unsigned int);

int asn1decode_ActionSlot(FILE *, FILE *, int);
bool is_ActionSlot(unsigned char, unsigned int);

int asn1decode_NoTokenActionSlots(FILE *, FILE *, int);
bool is_NoTokenActionSlots(unsigned char, unsigned int);

int asn1decode_ActionSlotSeq(FILE *, FILE *, int);
bool is_ActionSlotSeq(unsigned char, unsigned int);

int asn1decode_ActionClassSeq(FILE *, FILE *, int);
bool is_ActionClassSeq(unsigned char, unsigned int);

int asn1decode_ListGroupClass(FILE *, FILE *, int);
bool is_ListGroupClass(unsigned char, unsigned int);

int asn1decode_Positions(FILE *, FILE *, int);
bool is_Positions(unsigned char, unsigned int);

int asn1decode_Position(FILE *, FILE *, int);
bool is_Position(unsigned char, unsigned int);

int asn1decode_WrapAround(FILE *, FILE *, int);
bool is_WrapAround(unsigned char, unsigned int);

int asn1decode_MultipleSelection(FILE *, FILE *, int);
bool is_MultipleSelection(unsigned char, unsigned int);

int asn1decode_Visible(FILE *, FILE *, int);
bool is_Visible(unsigned char, unsigned int);

int asn1decode_OriginalBoxSize(FILE *, FILE *, int);
bool is_OriginalBoxSize(unsigned char, unsigned int);

int asn1decode_BoxSize(FILE *, FILE *, int);
bool is_BoxSize(unsigned char, unsigned int);

int asn1decode_XLength(FILE *, FILE *, int);
bool is_XLength(unsigned char, unsigned int);

int asn1decode_YLength(FILE *, FILE *, int);
bool is_YLength(unsigned char, unsigned int);

int asn1decode_OriginalPosition(FILE *, FILE *, int);
bool is_OriginalPosition(unsigned char, unsigned int);

int asn1decode_OriginalPaletteRef(FILE *, FILE *, int);
bool is_OriginalPaletteRef(unsigned char, unsigned int);

int asn1decode_BitmapClass(FILE *, FILE *, int);
bool is_BitmapClass(unsigned char, unsigned int);

int asn1decode_Tiling(FILE *, FILE *, int);
bool is_Tiling(unsigned char, unsigned int);

int asn1decode_OriginalTransparency(FILE *, FILE *, int);
bool is_OriginalTransparency(unsigned char, unsigned int);

int asn1decode_LineArtClass(FILE *, FILE *, int);
bool is_LineArtClass(unsigned char, unsigned int);

int asn1decode_LineArtBody(FILE *, FILE *, int);
bool is_LineArtBody(unsigned char, unsigned int);

int asn1decode_BorderedBoundingBox(FILE *, FILE *, int);
bool is_BorderedBoundingBox(unsigned char, unsigned int);

int asn1decode_OriginalLineWidth(FILE *, FILE *, int);
bool is_OriginalLineWidth(unsigned char, unsigned int);

int asn1decode_OriginalLineStyle(FILE *, FILE *, int);
bool is_OriginalLineStyle(unsigned char, unsigned int);

int asn1decode_OriginalRefLineColour(FILE *, FILE *, int);
bool is_OriginalRefLineColour(unsigned char, unsigned int);

int asn1decode_OriginalRefFillColour(FILE *, FILE *, int);
bool is_OriginalRefFillColour(unsigned char, unsigned int);

int asn1decode_RectangleClass(FILE *, FILE *, int);
bool is_RectangleClass(unsigned char, unsigned int);

int asn1decode_DynamicLineArtClass(FILE *, FILE *, int);
bool is_DynamicLineArtClass(unsigned char, unsigned int);

int asn1decode_TextClass(FILE *, FILE *, int);
bool is_TextClass(unsigned char, unsigned int);

int asn1decode_TextBody(FILE *, FILE *, int);
bool is_TextBody(unsigned char, unsigned int);

int asn1decode_OriginalFont(FILE *, FILE *, int);
bool is_OriginalFont(unsigned char, unsigned int);

int asn1decode_HorizontalJustification(FILE *, FILE *, int);
bool is_HorizontalJustification(unsigned char, unsigned int);

int asn1decode_JustificationEnum(FILE *, FILE *, int);
bool is_JustificationEnum(unsigned char, unsigned int);

int der_decode_JustificationEnum(FILE *, FILE *, int);
int asn1decode_VerticalJustification(FILE *, FILE *, int);
bool is_VerticalJustification(unsigned char, unsigned int);

int asn1decode_LineOrientation(FILE *, FILE *, int);
bool is_LineOrientation(unsigned char, unsigned int);

int asn1decode_LineOrientationEnum(FILE *, FILE *, int);
bool is_LineOrientationEnum(unsigned char, unsigned int);

int der_decode_LineOrientationEnum(FILE *, FILE *, int);
int asn1decode_StartCorner(FILE *, FILE *, int);
bool is_StartCorner(unsigned char, unsigned int);

int asn1decode_StartCornerEnum(FILE *, FILE *, int);
bool is_StartCornerEnum(unsigned char, unsigned int);

int der_decode_StartCornerEnum(FILE *, FILE *, int);
int asn1decode_TextWrapping(FILE *, FILE *, int);
bool is_TextWrapping(unsigned char, unsigned int);

int asn1decode_StreamClass(FILE *, FILE *, int);
bool is_StreamClass(unsigned char, unsigned int);

int asn1decode_Multiplex(FILE *, FILE *, int);
bool is_Multiplex(unsigned char, unsigned int);

int asn1decode_StreamComponent(FILE *, FILE *, int);
bool is_StreamComponent(unsigned char, unsigned int);

int asn1decode_Storage(FILE *, FILE *, int);
bool is_Storage(unsigned char, unsigned int);

int asn1decode_StorageEnum(FILE *, FILE *, int);
bool is_StorageEnum(unsigned char, unsigned int);

int der_decode_StorageEnum(FILE *, FILE *, int);
int asn1decode_Looping(FILE *, FILE *, int);
bool is_Looping(unsigned char, unsigned int);

int asn1decode_AudioClass(FILE *, FILE *, int);
bool is_AudioClass(unsigned char, unsigned int);

int asn1decode_ComponentTag(FILE *, FILE *, int);
bool is_ComponentTag(unsigned char, unsigned int);

int asn1decode_OriginalVolume(FILE *, FILE *, int);
bool is_OriginalVolume(unsigned char, unsigned int);

int asn1decode_VideoClass(FILE *, FILE *, int);
bool is_VideoClass(unsigned char, unsigned int);

int asn1decode_Termination(FILE *, FILE *, int);
bool is_Termination(unsigned char, unsigned int);

int asn1decode_TerminationEnum(FILE *, FILE *, int);
bool is_TerminationEnum(unsigned char, unsigned int);

int der_decode_TerminationEnum(FILE *, FILE *, int);
int asn1decode_RTGraphicsClass(FILE *, FILE *, int);
bool is_RTGraphicsClass(unsigned char, unsigned int);

int asn1decode_Interactible(FILE *, FILE *, int);
bool is_Interactible(unsigned char, unsigned int);

int asn1decode_EngineResp(FILE *, FILE *, int);
bool is_EngineResp(unsigned char, unsigned int);

int asn1decode_SliderClass(FILE *, FILE *, int);
bool is_SliderClass(unsigned char, unsigned int);

int asn1decode_Orientation(FILE *, FILE *, int);
bool is_Orientation(unsigned char, unsigned int);

int asn1decode_OrientationEnum(FILE *, FILE *, int);
bool is_OrientationEnum(unsigned char, unsigned int);

int der_decode_OrientationEnum(FILE *, FILE *, int);
int asn1decode_MaxValue(FILE *, FILE *, int);
bool is_MaxValue(unsigned char, unsigned int);

int asn1decode_MinValue(FILE *, FILE *, int);
bool is_MinValue(unsigned char, unsigned int);

int asn1decode_InitialValue(FILE *, FILE *, int);
bool is_InitialValue(unsigned char, unsigned int);

int asn1decode_InitialPortion(FILE *, FILE *, int);
bool is_InitialPortion(unsigned char, unsigned int);

int asn1decode_StepSize(FILE *, FILE *, int);
bool is_StepSize(unsigned char, unsigned int);

int asn1decode_SliderStyle(FILE *, FILE *, int);
bool is_SliderStyle(unsigned char, unsigned int);

int asn1decode_SliderStyleEnum(FILE *, FILE *, int);
bool is_SliderStyleEnum(unsigned char, unsigned int);

int der_decode_SliderStyleEnum(FILE *, FILE *, int);
int asn1decode_EntryFieldClass(FILE *, FILE *, int);
bool is_EntryFieldClass(unsigned char, unsigned int);

int asn1decode_InputType(FILE *, FILE *, int);
bool is_InputType(unsigned char, unsigned int);

int asn1decode_InputTypeEnum(FILE *, FILE *, int);
bool is_InputTypeEnum(unsigned char, unsigned int);

int der_decode_InputTypeEnum(FILE *, FILE *, int);
int asn1decode_CharList(FILE *, FILE *, int);
bool is_CharList(unsigned char, unsigned int);

int asn1decode_ObscuredInput(FILE *, FILE *, int);
bool is_ObscuredInput(unsigned char, unsigned int);

int asn1decode_MaxLength(FILE *, FILE *, int);
bool is_MaxLength(unsigned char, unsigned int);

int asn1decode_HyperTextClass(FILE *, FILE *, int);
bool is_HyperTextClass(unsigned char, unsigned int);

int asn1decode_Button(FILE *, FILE *, int);
bool is_Button(unsigned char, unsigned int);

int asn1decode_HotspotClass(FILE *, FILE *, int);
bool is_HotspotClass(unsigned char, unsigned int);

int asn1decode_PushButtonClass(FILE *, FILE *, int);
bool is_PushButtonClass(unsigned char, unsigned int);

int asn1decode_PushButtonBody(FILE *, FILE *, int);
bool is_PushButtonBody(unsigned char, unsigned int);

int asn1decode_OriginalLabel(FILE *, FILE *, int);
bool is_OriginalLabel(unsigned char, unsigned int);

int asn1decode_SwitchButtonClass(FILE *, FILE *, int);
bool is_SwitchButtonClass(unsigned char, unsigned int);

int asn1decode_ButtonStyle(FILE *, FILE *, int);
bool is_ButtonStyle(unsigned char, unsigned int);

int asn1decode_ButtonStyleEnum(FILE *, FILE *, int);
bool is_ButtonStyleEnum(unsigned char, unsigned int);

int der_decode_ButtonStyleEnum(FILE *, FILE *, int);
int asn1decode_ActionClass(FILE *, FILE *, int);
bool is_ActionClass(unsigned char, unsigned int);

int asn1decode_ElementaryAction(FILE *, FILE *, int);
bool is_ElementaryAction(unsigned char, unsigned int);

int asn1decode_Activate(FILE *, FILE *, int);
bool is_Activate(unsigned char, unsigned int);

int asn1decode_Add(FILE *, FILE *, int);
bool is_Add(unsigned char, unsigned int);

int asn1decode_AddItem(FILE *, FILE *, int);
bool is_AddItem(unsigned char, unsigned int);

int asn1decode_Append(FILE *, FILE *, int);
bool is_Append(unsigned char, unsigned int);

int asn1decode_BringToFront(FILE *, FILE *, int);
bool is_BringToFront(unsigned char, unsigned int);

int asn1decode_Call(FILE *, FILE *, int);
bool is_Call(unsigned char, unsigned int);

int asn1decode_CallActionSlot(FILE *, FILE *, int);
bool is_CallActionSlot(unsigned char, unsigned int);

int asn1decode_Clear(FILE *, FILE *, int);
bool is_Clear(unsigned char, unsigned int);

int asn1decode_Clone(FILE *, FILE *, int);
bool is_Clone(unsigned char, unsigned int);

int asn1decode_CloseConnection(FILE *, FILE *, int);
bool is_CloseConnection(unsigned char, unsigned int);

int asn1decode_Deactivate(FILE *, FILE *, int);
bool is_Deactivate(unsigned char, unsigned int);

int asn1decode_DelItem(FILE *, FILE *, int);
bool is_DelItem(unsigned char, unsigned int);

int asn1decode_Deselect(FILE *, FILE *, int);
bool is_Deselect(unsigned char, unsigned int);

int asn1decode_DeselectItem(FILE *, FILE *, int);
bool is_DeselectItem(unsigned char, unsigned int);

int asn1decode_Divide(FILE *, FILE *, int);
bool is_Divide(unsigned char, unsigned int);

int asn1decode_DrawArc(FILE *, FILE *, int);
bool is_DrawArc(unsigned char, unsigned int);

int asn1decode_DrawLine(FILE *, FILE *, int);
bool is_DrawLine(unsigned char, unsigned int);

int asn1decode_DrawOval(FILE *, FILE *, int);
bool is_DrawOval(unsigned char, unsigned int);

int asn1decode_DrawPolygon(FILE *, FILE *, int);
bool is_DrawPolygon(unsigned char, unsigned int);

int asn1decode_DrawPolyline(FILE *, FILE *, int);
bool is_DrawPolyline(unsigned char, unsigned int);

int asn1decode_DrawRectangle(FILE *, FILE *, int);
bool is_DrawRectangle(unsigned char, unsigned int);

int asn1decode_DrawSector(FILE *, FILE *, int);
bool is_DrawSector(unsigned char, unsigned int);

int asn1decode_Fork(FILE *, FILE *, int);
bool is_Fork(unsigned char, unsigned int);

int asn1decode_GetAvailabilityStatus(FILE *, FILE *, int);
bool is_GetAvailabilityStatus(unsigned char, unsigned int);

int asn1decode_GetBoxSize(FILE *, FILE *, int);
bool is_GetBoxSize(unsigned char, unsigned int);

int asn1decode_GetCellItem(FILE *, FILE *, int);
bool is_GetCellItem(unsigned char, unsigned int);

int asn1decode_GetCursorPosition(FILE *, FILE *, int);
bool is_GetCursorPosition(unsigned char, unsigned int);

int asn1decode_GetEngineSupport(FILE *, FILE *, int);
bool is_GetEngineSupport(unsigned char, unsigned int);

int asn1decode_GetEntryPoint(FILE *, FILE *, int);
bool is_GetEntryPoint(unsigned char, unsigned int);

int asn1decode_GetFillColour(FILE *, FILE *, int);
bool is_GetFillColour(unsigned char, unsigned int);

int asn1decode_GetFirstItem(FILE *, FILE *, int);
bool is_GetFirstItem(unsigned char, unsigned int);

int asn1decode_GetHighlightStatus(FILE *, FILE *, int);
bool is_GetHighlightStatus(unsigned char, unsigned int);

int asn1decode_GetInteractionStatus(FILE *, FILE *, int);
bool is_GetInteractionStatus(unsigned char, unsigned int);

int asn1decode_GetItemStatus(FILE *, FILE *, int);
bool is_GetItemStatus(unsigned char, unsigned int);

int asn1decode_GetLabel(FILE *, FILE *, int);
bool is_GetLabel(unsigned char, unsigned int);

int asn1decode_GetLastAnchorFired(FILE *, FILE *, int);
bool is_GetLastAnchorFired(unsigned char, unsigned int);

int asn1decode_GetLineColour(FILE *, FILE *, int);
bool is_GetLineColour(unsigned char, unsigned int);

int asn1decode_GetLineStyle(FILE *, FILE *, int);
bool is_GetLineStyle(unsigned char, unsigned int);

int asn1decode_GetLineWidth(FILE *, FILE *, int);
bool is_GetLineWidth(unsigned char, unsigned int);

int asn1decode_GetListItem(FILE *, FILE *, int);
bool is_GetListItem(unsigned char, unsigned int);

int asn1decode_GetListSize(FILE *, FILE *, int);
bool is_GetListSize(unsigned char, unsigned int);

int asn1decode_GetOverwriteMode(FILE *, FILE *, int);
bool is_GetOverwriteMode(unsigned char, unsigned int);

int asn1decode_GetPortion(FILE *, FILE *, int);
bool is_GetPortion(unsigned char, unsigned int);

int asn1decode_GetPosition(FILE *, FILE *, int);
bool is_GetPosition(unsigned char, unsigned int);

int asn1decode_GetRunningStatus(FILE *, FILE *, int);
bool is_GetRunningStatus(unsigned char, unsigned int);

int asn1decode_GetSelectionStatus(FILE *, FILE *, int);
bool is_GetSelectionStatus(unsigned char, unsigned int);

int asn1decode_GetSliderValue(FILE *, FILE *, int);
bool is_GetSliderValue(unsigned char, unsigned int);

int asn1decode_GetTextContent(FILE *, FILE *, int);
bool is_GetTextContent(unsigned char, unsigned int);

int asn1decode_GetTextData(FILE *, FILE *, int);
bool is_GetTextData(unsigned char, unsigned int);

int asn1decode_GetTokenPosition(FILE *, FILE *, int);
bool is_GetTokenPosition(unsigned char, unsigned int);

int asn1decode_GetVolume(FILE *, FILE *, int);
bool is_GetVolume(unsigned char, unsigned int);

int asn1decode_Launch(FILE *, FILE *, int);
bool is_Launch(unsigned char, unsigned int);

int asn1decode_LockScreen(FILE *, FILE *, int);
bool is_LockScreen(unsigned char, unsigned int);

int asn1decode_Modulo(FILE *, FILE *, int);
bool is_Modulo(unsigned char, unsigned int);

int asn1decode_Move(FILE *, FILE *, int);
bool is_Move(unsigned char, unsigned int);

int asn1decode_MoveTo(FILE *, FILE *, int);
bool is_MoveTo(unsigned char, unsigned int);

int asn1decode_Multiply(FILE *, FILE *, int);
bool is_Multiply(unsigned char, unsigned int);

int asn1decode_OpenConnection(FILE *, FILE *, int);
bool is_OpenConnection(unsigned char, unsigned int);

int asn1decode_Preload(FILE *, FILE *, int);
bool is_Preload(unsigned char, unsigned int);

int asn1decode_PutBefore(FILE *, FILE *, int);
bool is_PutBefore(unsigned char, unsigned int);

int asn1decode_PutBehind(FILE *, FILE *, int);
bool is_PutBehind(unsigned char, unsigned int);

int asn1decode_Quit(FILE *, FILE *, int);
bool is_Quit(unsigned char, unsigned int);

int asn1decode_ReadPersistent(FILE *, FILE *, int);
bool is_ReadPersistent(unsigned char, unsigned int);

int asn1decode_Run(FILE *, FILE *, int);
bool is_Run(unsigned char, unsigned int);

int asn1decode_ScaleBitmap(FILE *, FILE *, int);
bool is_ScaleBitmap(unsigned char, unsigned int);

int asn1decode_ScaleVideo(FILE *, FILE *, int);
bool is_ScaleVideo(unsigned char, unsigned int);

int asn1decode_ScrollItems(FILE *, FILE *, int);
bool is_ScrollItems(unsigned char, unsigned int);

int asn1decode_Select(FILE *, FILE *, int);
bool is_Select(unsigned char, unsigned int);

int asn1decode_SelectItem(FILE *, FILE *, int);
bool is_SelectItem(unsigned char, unsigned int);

int asn1decode_SendEvent(FILE *, FILE *, int);
bool is_SendEvent(unsigned char, unsigned int);

int asn1decode_SendToBack(FILE *, FILE *, int);
bool is_SendToBack(unsigned char, unsigned int);

int asn1decode_SetBoxSize(FILE *, FILE *, int);
bool is_SetBoxSize(unsigned char, unsigned int);

int asn1decode_SetCachePriority(FILE *, FILE *, int);
bool is_SetCachePriority(unsigned char, unsigned int);

int asn1decode_SetCounterEndPosition(FILE *, FILE *, int);
bool is_SetCounterEndPosition(unsigned char, unsigned int);

int asn1decode_SetCounterPosition(FILE *, FILE *, int);
bool is_SetCounterPosition(unsigned char, unsigned int);

int asn1decode_SetCounterTrigger(FILE *, FILE *, int);
bool is_SetCounterTrigger(unsigned char, unsigned int);

int asn1decode_SetCursorPosition(FILE *, FILE *, int);
bool is_SetCursorPosition(unsigned char, unsigned int);

int asn1decode_SetCursorShape(FILE *, FILE *, int);
bool is_SetCursorShape(unsigned char, unsigned int);

int asn1decode_SetData(FILE *, FILE *, int);
bool is_SetData(unsigned char, unsigned int);

int asn1decode_SetEntryPoint(FILE *, FILE *, int);
bool is_SetEntryPoint(unsigned char, unsigned int);

int asn1decode_SetFillColour(FILE *, FILE *, int);
bool is_SetFillColour(unsigned char, unsigned int);

int asn1decode_SetFirstItem(FILE *, FILE *, int);
bool is_SetFirstItem(unsigned char, unsigned int);

int asn1decode_SetFontRef(FILE *, FILE *, int);
bool is_SetFontRef(unsigned char, unsigned int);

int asn1decode_SetHighlightStatus(FILE *, FILE *, int);
bool is_SetHighlightStatus(unsigned char, unsigned int);

int asn1decode_SetInteractionStatus(FILE *, FILE *, int);
bool is_SetInteractionStatus(unsigned char, unsigned int);

int asn1decode_SetLabel(FILE *, FILE *, int);
bool is_SetLabel(unsigned char, unsigned int);

int asn1decode_SetLineColour(FILE *, FILE *, int);
bool is_SetLineColour(unsigned char, unsigned int);

int asn1decode_SetLineStyle(FILE *, FILE *, int);
bool is_SetLineStyle(unsigned char, unsigned int);

int asn1decode_SetLineWidth(FILE *, FILE *, int);
bool is_SetLineWidth(unsigned char, unsigned int);

int asn1decode_SetOverwriteMode(FILE *, FILE *, int);
bool is_SetOverwriteMode(unsigned char, unsigned int);

int asn1decode_SetPaletteRef(FILE *, FILE *, int);
bool is_SetPaletteRef(unsigned char, unsigned int);

int asn1decode_SetPortion(FILE *, FILE *, int);
bool is_SetPortion(unsigned char, unsigned int);

int asn1decode_SetPosition(FILE *, FILE *, int);
bool is_SetPosition(unsigned char, unsigned int);

int asn1decode_SetSliderValue(FILE *, FILE *, int);
bool is_SetSliderValue(unsigned char, unsigned int);

int asn1decode_SetSpeed(FILE *, FILE *, int);
bool is_SetSpeed(unsigned char, unsigned int);

int asn1decode_SetTimer(FILE *, FILE *, int);
bool is_SetTimer(unsigned char, unsigned int);

int asn1decode_NewTimer(FILE *, FILE *, int);
bool is_NewTimer(unsigned char, unsigned int);

int asn1decode_SetTransparency(FILE *, FILE *, int);
bool is_SetTransparency(unsigned char, unsigned int);

int asn1decode_SetVariable(FILE *, FILE *, int);
bool is_SetVariable(unsigned char, unsigned int);

int asn1decode_SetVolume(FILE *, FILE *, int);
bool is_SetVolume(unsigned char, unsigned int);

int asn1decode_Spawn(FILE *, FILE *, int);
bool is_Spawn(unsigned char, unsigned int);

int asn1decode_Stop(FILE *, FILE *, int);
bool is_Stop(unsigned char, unsigned int);

int asn1decode_Step(FILE *, FILE *, int);
bool is_Step(unsigned char, unsigned int);

int asn1decode_StorePersistent(FILE *, FILE *, int);
bool is_StorePersistent(unsigned char, unsigned int);

int asn1decode_Subtract(FILE *, FILE *, int);
bool is_Subtract(unsigned char, unsigned int);

int asn1decode_TestVariable(FILE *, FILE *, int);
bool is_TestVariable(unsigned char, unsigned int);

int asn1decode_Toggle(FILE *, FILE *, int);
bool is_Toggle(unsigned char, unsigned int);

int asn1decode_ToggleItem(FILE *, FILE *, int);
bool is_ToggleItem(unsigned char, unsigned int);

int asn1decode_TransitionTo(FILE *, FILE *, int);
bool is_TransitionTo(unsigned char, unsigned int);

int asn1decode_Unload(FILE *, FILE *, int);
bool is_Unload(unsigned char, unsigned int);

int asn1decode_UnlockScreen(FILE *, FILE *, int);
bool is_UnlockScreen(unsigned char, unsigned int);

int asn1decode_SetBackgroundColour(FILE *, FILE *, int);
bool is_SetBackgroundColour(unsigned char, unsigned int);

int asn1decode_SetCellPosition(FILE *, FILE *, int);
bool is_SetCellPosition(unsigned char, unsigned int);

int asn1decode_SetInputReg(FILE *, FILE *, int);
bool is_SetInputReg(unsigned char, unsigned int);

int asn1decode_SetTextColour(FILE *, FILE *, int);
bool is_SetTextColour(unsigned char, unsigned int);

int asn1decode_SetFontAttributes(FILE *, FILE *, int);
bool is_SetFontAttributes(unsigned char, unsigned int);

int asn1decode_SetVideoDecodeOffset(FILE *, FILE *, int);
bool is_SetVideoDecodeOffset(unsigned char, unsigned int);

int asn1decode_GetVideoDecodeOffset(FILE *, FILE *, int);
bool is_GetVideoDecodeOffset(unsigned char, unsigned int);

int asn1decode_GetFocusPosition(FILE *, FILE *, int);
bool is_GetFocusPosition(unsigned char, unsigned int);

int asn1decode_SetFocusPosition(FILE *, FILE *, int);
bool is_SetFocusPosition(unsigned char, unsigned int);

int asn1decode_SetBitmapDecodeOffset(FILE *, FILE *, int);
bool is_SetBitmapDecodeOffset(unsigned char, unsigned int);

int asn1decode_GetBitmapDecodeOffset(FILE *, FILE *, int);
bool is_GetBitmapDecodeOffset(unsigned char, unsigned int);

int asn1decode_SetSliderParameters(FILE *, FILE *, int);
bool is_SetSliderParameters(unsigned char, unsigned int);

int asn1decode_AbsoluteTime(FILE *, FILE *, int);
bool is_AbsoluteTime(unsigned char, unsigned int);

int asn1decode_Address(FILE *, FILE *, int);
bool is_Address(unsigned char, unsigned int);

int asn1decode_Answer(FILE *, FILE *, int);
bool is_Answer(unsigned char, unsigned int);

int asn1decode_AppendValue(FILE *, FILE *, int);
bool is_AppendValue(unsigned char, unsigned int);

int asn1decode_ArcAngle(FILE *, FILE *, int);
bool is_ArcAngle(unsigned char, unsigned int);

int asn1decode_AvailabilityStatusVar(FILE *, FILE *, int);
bool is_AvailabilityStatusVar(unsigned char, unsigned int);

int asn1decode_CallSucceeded(FILE *, FILE *, int);
bool is_CallSucceeded(unsigned char, unsigned int);

int asn1decode_CellIndex(FILE *, FILE *, int);
bool is_CellIndex(unsigned char, unsigned int);

int asn1decode_CloneRefVar(FILE *, FILE *, int);
bool is_CloneRefVar(unsigned char, unsigned int);

int asn1decode_ComparisonValue(FILE *, FILE *, int);
bool is_ComparisonValue(unsigned char, unsigned int);

int asn1decode_ConnectionTag(FILE *, FILE *, int);
bool is_ConnectionTag(unsigned char, unsigned int);

int asn1decode_Denominator(FILE *, FILE *, int);
bool is_Denominator(unsigned char, unsigned int);

int asn1decode_EllipseHeight(FILE *, FILE *, int);
bool is_EllipseHeight(unsigned char, unsigned int);

int asn1decode_EllipseWidth(FILE *, FILE *, int);
bool is_EllipseWidth(unsigned char, unsigned int);

int asn1decode_EmulatedEventData(FILE *, FILE *, int);
bool is_EmulatedEventData(unsigned char, unsigned int);

int asn1decode_EmulatedEventSource(FILE *, FILE *, int);
bool is_EmulatedEventSource(unsigned char, unsigned int);

int asn1decode_EmulatedEventType(FILE *, FILE *, int);
bool is_EmulatedEventType(unsigned char, unsigned int);

int asn1decode_EntryPointVar(FILE *, FILE *, int);
bool is_EntryPointVar(unsigned char, unsigned int);

int asn1decode_ForkSucceeded(FILE *, FILE *, int);
bool is_ForkSucceeded(unsigned char, unsigned int);

int asn1decode_Feature(FILE *, FILE *, int);
bool is_Feature(unsigned char, unsigned int);

int asn1decode_FillColourVar(FILE *, FILE *, int);
bool is_FillColourVar(unsigned char, unsigned int);

int asn1decode_FirstItemVar(FILE *, FILE *, int);
bool is_FirstItemVar(unsigned char, unsigned int);

int asn1decode_HighlightStatusVar(FILE *, FILE *, int);
bool is_HighlightStatusVar(unsigned char, unsigned int);

int asn1decode_Index(FILE *, FILE *, int);
bool is_Index(unsigned char, unsigned int);

int asn1decode_InFileName(FILE *, FILE *, int);
bool is_InFileName(unsigned char, unsigned int);

int asn1decode_InteractionStatusVar(FILE *, FILE *, int);
bool is_InteractionStatusVar(unsigned char, unsigned int);

int asn1decode_InVariables(FILE *, FILE *, int);
bool is_InVariables(unsigned char, unsigned int);

int asn1decode_ItemIndex(FILE *, FILE *, int);
bool is_ItemIndex(unsigned char, unsigned int);

int asn1decode_ItemRefVar(FILE *, FILE *, int);
bool is_ItemRefVar(unsigned char, unsigned int);

int asn1decode_ItemStatusVar(FILE *, FILE *, int);
bool is_ItemStatusVar(unsigned char, unsigned int);

int asn1decode_ItemsToScroll(FILE *, FILE *, int);
bool is_ItemsToScroll(unsigned char, unsigned int);

int asn1decode_LabelVar(FILE *, FILE *, int);
bool is_LabelVar(unsigned char, unsigned int);

int asn1decode_LastAnchorFiredVar(FILE *, FILE *, int);
bool is_LastAnchorFiredVar(unsigned char, unsigned int);

int asn1decode_LineColourVar(FILE *, FILE *, int);
bool is_LineColourVar(unsigned char, unsigned int);

int asn1decode_LineStyleVar(FILE *, FILE *, int);
bool is_LineStyleVar(unsigned char, unsigned int);

int asn1decode_LineWidthVar(FILE *, FILE *, int);
bool is_LineWidthVar(unsigned char, unsigned int);

int asn1decode_MovementIdentifier(FILE *, FILE *, int);
bool is_MovementIdentifier(unsigned char, unsigned int);

int asn1decode_NbOfSteps(FILE *, FILE *, int);
bool is_NbOfSteps(unsigned char, unsigned int);

int asn1decode_NewAbsoluteColour(FILE *, FILE *, int);
bool is_NewAbsoluteColour(unsigned char, unsigned int);

int asn1decode_NewCachePriority(FILE *, FILE *, int);
bool is_NewCachePriority(unsigned char, unsigned int);

int asn1decode_NewColour(FILE *, FILE *, int);
bool is_NewColour(unsigned char, unsigned int);

int asn1decode_NewColourIndex(FILE *, FILE *, int);
bool is_NewColourIndex(unsigned char, unsigned int);

int asn1decode_NewContent(FILE *, FILE *, int);
bool is_NewContent(unsigned char, unsigned int);

int asn1decode_NewContentCachePriority(FILE *, FILE *, int);
bool is_NewContentCachePriority(unsigned char, unsigned int);

int asn1decode_NewCounterEndPosition(FILE *, FILE *, int);
bool is_NewCounterEndPosition(unsigned char, unsigned int);

int asn1decode_NewCounterPosition(FILE *, FILE *, int);
bool is_NewCounterPosition(unsigned char, unsigned int);

int asn1decode_NewContentSize(FILE *, FILE *, int);
bool is_NewContentSize(unsigned char, unsigned int);

int asn1decode_NewCounterValue(FILE *, FILE *, int);
bool is_NewCounterValue(unsigned char, unsigned int);

int asn1decode_NewCursorShape(FILE *, FILE *, int);
bool is_NewCursorShape(unsigned char, unsigned int);

int asn1decode_NewEntryPoint(FILE *, FILE *, int);
bool is_NewEntryPoint(unsigned char, unsigned int);

int asn1decode_NewFirstItem(FILE *, FILE *, int);
bool is_NewFirstItem(unsigned char, unsigned int);

int asn1decode_NewFont(FILE *, FILE *, int);
bool is_NewFont(unsigned char, unsigned int);

int asn1decode_NewFontName(FILE *, FILE *, int);
bool is_NewFontName(unsigned char, unsigned int);

int asn1decode_NewFontReference(FILE *, FILE *, int);
bool is_NewFontReference(unsigned char, unsigned int);

int asn1decode_NewGenericBoolean(FILE *, FILE *, int);
bool is_NewGenericBoolean(unsigned char, unsigned int);

int asn1decode_NewGenericInteger(FILE *, FILE *, int);
bool is_NewGenericInteger(unsigned char, unsigned int);

int asn1decode_NewGenericOctetString(FILE *, FILE *, int);
bool is_NewGenericOctetString(unsigned char, unsigned int);

int asn1decode_NewGenericObjectReference(FILE *, FILE *, int);
bool is_NewGenericObjectReference(unsigned char, unsigned int);

int asn1decode_NewGenericContentReference(FILE *, FILE *, int);
bool is_NewGenericContentReference(unsigned char, unsigned int);

int asn1decode_NewHighlightStatus(FILE *, FILE *, int);
bool is_NewHighlightStatus(unsigned char, unsigned int);

int asn1decode_NewIncludedContent(FILE *, FILE *, int);
bool is_NewIncludedContent(unsigned char, unsigned int);

int asn1decode_NewInteractionStatus(FILE *, FILE *, int);
bool is_NewInteractionStatus(unsigned char, unsigned int);

int asn1decode_NewLabel(FILE *, FILE *, int);
bool is_NewLabel(unsigned char, unsigned int);

int asn1decode_NewLineStyle(FILE *, FILE *, int);
bool is_NewLineStyle(unsigned char, unsigned int);

int asn1decode_NewLineWidth(FILE *, FILE *, int);
bool is_NewLineWidth(unsigned char, unsigned int);

int asn1decode_NewOverwriteMode(FILE *, FILE *, int);
bool is_NewOverwriteMode(unsigned char, unsigned int);

int asn1decode_NewPaletteRef(FILE *, FILE *, int);
bool is_NewPaletteRef(unsigned char, unsigned int);

int asn1decode_NewPortion(FILE *, FILE *, int);
bool is_NewPortion(unsigned char, unsigned int);

int asn1decode_NewReferencedContent(FILE *, FILE *, int);
bool is_NewReferencedContent(unsigned char, unsigned int);

int asn1decode_NewSliderValue(FILE *, FILE *, int);
bool is_NewSliderValue(unsigned char, unsigned int);

int asn1decode_NewSpeed(FILE *, FILE *, int);
bool is_NewSpeed(unsigned char, unsigned int);

int asn1decode_NewTransparency(FILE *, FILE *, int);
bool is_NewTransparency(unsigned char, unsigned int);

int asn1decode_NewVariableValue(FILE *, FILE *, int);
bool is_NewVariableValue(unsigned char, unsigned int);

int asn1decode_NewVolume(FILE *, FILE *, int);
bool is_NewVolume(unsigned char, unsigned int);

int asn1decode_NewXPosition(FILE *, FILE *, int);
bool is_NewXPosition(unsigned char, unsigned int);

int asn1decode_NewYPosition(FILE *, FILE *, int);
bool is_NewYPosition(unsigned char, unsigned int);

int asn1decode_Numerator(FILE *, FILE *, int);
bool is_Numerator(unsigned char, unsigned int);

int asn1decode_OpenSucceeded(FILE *, FILE *, int);
bool is_OpenSucceeded(unsigned char, unsigned int);

int asn1decode_Operator(FILE *, FILE *, int);
bool is_Operator(unsigned char, unsigned int);

int asn1decode_OutFileName(FILE *, FILE *, int);
bool is_OutFileName(unsigned char, unsigned int);

int asn1decode_OutVariables(FILE *, FILE *, int);
bool is_OutVariables(unsigned char, unsigned int);

int asn1decode_OverwriteModeVar(FILE *, FILE *, int);
bool is_OverwriteModeVar(unsigned char, unsigned int);

int asn1decode_Parameter(FILE *, FILE *, int);
bool is_Parameter(unsigned char, unsigned int);

int asn1decode_Parameters(FILE *, FILE *, int);
bool is_Parameters(unsigned char, unsigned int);

int asn1decode_Point(FILE *, FILE *, int);
bool is_Point(unsigned char, unsigned int);

int asn1decode_PointList(FILE *, FILE *, int);
bool is_PointList(unsigned char, unsigned int);

int asn1decode_PortionVar(FILE *, FILE *, int);
bool is_PortionVar(unsigned char, unsigned int);

int asn1decode_Protocol(FILE *, FILE *, int);
bool is_Protocol(unsigned char, unsigned int);

int asn1decode_Rational(FILE *, FILE *, int);
bool is_Rational(unsigned char, unsigned int);

int asn1decode_ReadSucceeded(FILE *, FILE *, int);
bool is_ReadSucceeded(unsigned char, unsigned int);

int asn1decode_ReferenceVisible(FILE *, FILE *, int);
bool is_ReferenceVisible(unsigned char, unsigned int);

int asn1decode_RunningStatusVar(FILE *, FILE *, int);
bool is_RunningStatusVar(unsigned char, unsigned int);

int asn1decode_SelectionStatusVar(FILE *, FILE *, int);
bool is_SelectionStatusVar(unsigned char, unsigned int);

int asn1decode_SizeVar(FILE *, FILE *, int);
bool is_SizeVar(unsigned char, unsigned int);

int asn1decode_SliderValueVar(FILE *, FILE *, int);
bool is_SliderValueVar(unsigned char, unsigned int);

int asn1decode_StartAngle(FILE *, FILE *, int);
bool is_StartAngle(unsigned char, unsigned int);

int asn1decode_StoreSucceeded(FILE *, FILE *, int);
bool is_StoreSucceeded(unsigned char, unsigned int);

int asn1decode_Target(FILE *, FILE *, int);
bool is_Target(unsigned char, unsigned int);

int asn1decode_TextContentVar(FILE *, FILE *, int);
bool is_TextContentVar(unsigned char, unsigned int);

int asn1decode_TextDataVar(FILE *, FILE *, int);
bool is_TextDataVar(unsigned char, unsigned int);

int asn1decode_TimerID(FILE *, FILE *, int);
bool is_TimerID(unsigned char, unsigned int);

int asn1decode_TimerValue(FILE *, FILE *, int);
bool is_TimerValue(unsigned char, unsigned int);

int asn1decode_TokenPositionVar(FILE *, FILE *, int);
bool is_TokenPositionVar(unsigned char, unsigned int);

int asn1decode_TransitionEffect(FILE *, FILE *, int);
bool is_TransitionEffect(unsigned char, unsigned int);

int asn1decode_TriggerIdentifier(FILE *, FILE *, int);
bool is_TriggerIdentifier(unsigned char, unsigned int);

int asn1decode_Value(FILE *, FILE *, int);
bool is_Value(unsigned char, unsigned int);

int asn1decode_VisibleReference(FILE *, FILE *, int);
bool is_VisibleReference(unsigned char, unsigned int);

int asn1decode_VolumeVar(FILE *, FILE *, int);
bool is_VolumeVar(unsigned char, unsigned int);

int asn1decode_X(FILE *, FILE *, int);
bool is_X(unsigned char, unsigned int);

int asn1decode_X1(FILE *, FILE *, int);
bool is_X1(unsigned char, unsigned int);

int asn1decode_X2(FILE *, FILE *, int);
bool is_X2(unsigned char, unsigned int);

int asn1decode_XBoxSizeVar(FILE *, FILE *, int);
bool is_XBoxSizeVar(unsigned char, unsigned int);

int asn1decode_XCursor(FILE *, FILE *, int);
bool is_XCursor(unsigned char, unsigned int);

int asn1decode_XNewBoxSize(FILE *, FILE *, int);
bool is_XNewBoxSize(unsigned char, unsigned int);

int asn1decode_XOut(FILE *, FILE *, int);
bool is_XOut(unsigned char, unsigned int);

int asn1decode_XPositionVar(FILE *, FILE *, int);
bool is_XPositionVar(unsigned char, unsigned int);

int asn1decode_XScale(FILE *, FILE *, int);
bool is_XScale(unsigned char, unsigned int);

int asn1decode_Y(FILE *, FILE *, int);
bool is_Y(unsigned char, unsigned int);

int asn1decode_Y1(FILE *, FILE *, int);
bool is_Y1(unsigned char, unsigned int);

int asn1decode_Y2(FILE *, FILE *, int);
bool is_Y2(unsigned char, unsigned int);

int asn1decode_YBoxSizeVar(FILE *, FILE *, int);
bool is_YBoxSizeVar(unsigned char, unsigned int);

int asn1decode_YCursor(FILE *, FILE *, int);
bool is_YCursor(unsigned char, unsigned int);

int asn1decode_YNewBoxSize(FILE *, FILE *, int);
bool is_YNewBoxSize(unsigned char, unsigned int);

int asn1decode_YOut(FILE *, FILE *, int);
bool is_YOut(unsigned char, unsigned int);

int asn1decode_YPositionVar(FILE *, FILE *, int);
bool is_YPositionVar(unsigned char, unsigned int);

int asn1decode_YScale(FILE *, FILE *, int);
bool is_YScale(unsigned char, unsigned int);

int asn1decode_XOffsetVar(FILE *, FILE *, int);
bool is_XOffsetVar(unsigned char, unsigned int);

int asn1decode_YOffsetVar(FILE *, FILE *, int);
bool is_YOffsetVar(unsigned char, unsigned int);

int asn1decode_NewXOffset(FILE *, FILE *, int);
bool is_NewXOffset(unsigned char, unsigned int);

int asn1decode_NewYOffset(FILE *, FILE *, int);
bool is_NewYOffset(unsigned char, unsigned int);

int asn1decode_FocusPositionVar(FILE *, FILE *, int);
bool is_FocusPositionVar(unsigned char, unsigned int);

int asn1decode_NewFocusPosition(FILE *, FILE *, int);
bool is_NewFocusPosition(unsigned char, unsigned int);

int asn1decode_NewMinValue(FILE *, FILE *, int);
bool is_NewMinValue(unsigned char, unsigned int);

int asn1decode_NewMaxValue(FILE *, FILE *, int);
bool is_NewMaxValue(unsigned char, unsigned int);

int asn1decode_NewStepSize(FILE *, FILE *, int);
bool is_NewStepSize(unsigned char, unsigned int);

int asn1decode_ObjectReference(FILE *, FILE *, int);
bool is_ObjectReference(unsigned char, unsigned int);

int asn1decode_ExternalReference(FILE *, FILE *, int);
bool is_ExternalReference(unsigned char, unsigned int);

int asn1decode_InternalReference(FILE *, FILE *, int);
bool is_InternalReference(unsigned char, unsigned int);

int asn1decode_GroupIdentifier(FILE *, FILE *, int);
bool is_GroupIdentifier(unsigned char, unsigned int);

int asn1decode_ObjectNumber(FILE *, FILE *, int);
bool is_ObjectNumber(unsigned char, unsigned int);

int asn1decode_ContentReference(FILE *, FILE *, int);
bool is_ContentReference(unsigned char, unsigned int);

int asn1decode_ContentReference69(FILE *, FILE *, int);
bool is_ContentReference69(unsigned char, unsigned int);

int asn1decode_GenericObjectReference(FILE *, FILE *, int);
bool is_GenericObjectReference(unsigned char, unsigned int);

int asn1decode_DirectReference(FILE *, FILE *, int);
bool is_DirectReference(unsigned char, unsigned int);

int asn1decode_IndirectReference(FILE *, FILE *, int);
bool is_IndirectReference(unsigned char, unsigned int);

int asn1decode_GenericContentReference(FILE *, FILE *, int);
bool is_GenericContentReference(unsigned char, unsigned int);

int asn1decode_GenericInteger(FILE *, FILE *, int);
bool is_GenericInteger(unsigned char, unsigned int);

int asn1decode_GenericBoolean(FILE *, FILE *, int);
bool is_GenericBoolean(unsigned char, unsigned int);

int asn1decode_GenericOctetString(FILE *, FILE *, int);
bool is_GenericOctetString(unsigned char, unsigned int);

int asn1decode_OctetString(FILE *, FILE *, int);
bool is_OctetString(unsigned char, unsigned int);

int asn1decode_Colour(FILE *, FILE *, int);
bool is_Colour(unsigned char, unsigned int);

int asn1decode_ColourIndex(FILE *, FILE *, int);
bool is_ColourIndex(unsigned char, unsigned int);

int asn1decode_AbsoluteColour(FILE *, FILE *, int);
bool is_AbsoluteColour(unsigned char, unsigned int);

int asn1decode_XYPosition(FILE *, FILE *, int);
bool is_XYPosition(unsigned char, unsigned int);

int asn1decode_XPosition(FILE *, FILE *, int);
bool is_XPosition(unsigned char, unsigned int);

int asn1decode_YPosition(FILE *, FILE *, int);
bool is_YPosition(unsigned char, unsigned int);

#endif // hmkim : added.

#endif	/* __ASN1DECODE_H__ */

