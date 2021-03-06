enable_overlay_directory()

add_wn_library(librocket
  INCLUDES
     ${WNFramework_SOURCE_DIR}/Externals/stb
     ${CMAKE_CURRENT_SOURCE_DIR}/Include/
     ${CMAKE_CURRENT_SOURCE_DIR}/../
  SOURCES
    Include/Rocket/Controls/Clipboard.h
    Include/Rocket/Controls/Controls.h
    Include/Rocket/Controls/DataFormatter.h
    Include/Rocket/Controls/DataQuery.h
    Include/Rocket/Controls/DataSource.h
    Include/Rocket/Controls/DataSourceListener.h
    Include/Rocket/Controls/ElementDataGrid.h
    Include/Rocket/Controls/ElementDataGridCell.h
    Include/Rocket/Controls/ElementDataGridExpandButton.h
    Include/Rocket/Controls/ElementDataGridRow.h
    Include/Rocket/Controls/ElementForm.h
    Include/Rocket/Controls/ElementFormControl.h
    Include/Rocket/Controls/ElementFormControlDataSelect.h
    Include/Rocket/Controls/ElementFormControlInput.h
    Include/Rocket/Controls/ElementFormControlSelect.h
    Include/Rocket/Controls/ElementFormControlTextArea.h
    Include/Rocket/Controls/ElementTabSet.h
    Include/Rocket/Controls/SelectOption.h
    Include/Rocket/Controls.h
    Include/Rocket/Core/BaseXMLParser.h
    Include/Rocket/Core/BitmapFont/FontProvider.h
    Include/Rocket/Core/TTFFont/FontProvider.h
    Include/Rocket/Core/Box.h
    Include/Rocket/Core/Colour.h
    Include/Rocket/Core/Colour.inl
    Include/Rocket/Core/DocumentContext.h
    Include/Rocket/Core/DocumentContextInstancer.h
    Include/Rocket/Core/ConvolutionFilter.h
    Include/Rocket/Core/Core.h
    Include/Rocket/Core/Debug.h
    Include/Rocket/Core/Decorator.h
    Include/Rocket/Core/DecoratorInstancer.h
    Include/Rocket/Core/Dictionary.h
    Include/Rocket/Core/Dictionary.inl
    Include/Rocket/Core/Element.h
    Include/Rocket/Core/Element.inl
    Include/Rocket/Core/ElementDocument.h
    Include/Rocket/Core/ElementInstancer.h
    Include/Rocket/Core/ElementInstancerGeneric.h
    Include/Rocket/Core/ElementInstancerGeneric.inl
    Include/Rocket/Core/ElementReference.h
    Include/Rocket/Core/ElementScroll.h
    Include/Rocket/Core/ElementText.h
    Include/Rocket/Core/ElementUtilities.h
    Include/Rocket/Core/Event.h
    Include/Rocket/Core/EventInstancer.h
    Include/Rocket/Core/EventListener.h
    Include/Rocket/Core/EventListenerInstancer.h
    Include/Rocket/Core/Factory.h
    Include/Rocket/Core/FileInterface.h
    Include/Rocket/Core/Font.h
    Include/Rocket/Core/FontDatabase.h
    Include/Rocket/Core/FontEffect.h
    Include/Rocket/Core/FontEffectInstancer.h
    Include/Rocket/Core/FontFace.h
    Include/Rocket/Core/FontFamily.h
    Include/Rocket/Core/FontGlyph.h
    Include/Rocket/Core/FontProvider.h
    Include/Rocket/Core/Geometry.h
    Include/Rocket/Core/GeometryUtilities.h
    Include/Rocket/Core/Input.h
    Include/Rocket/Core/Log.h
    Include/Rocket/Core/Math.h
    Include/Rocket/Core/MathTypes.h
    Include/Rocket/Core/Platform.h
    Include/Rocket/Core/Plugin.h
    Include/Rocket/Core/Property.h
    Include/Rocket/Core/PropertyDefinition.h
    Include/Rocket/Core/PropertyDictionary.h
    Include/Rocket/Core/PropertyParser.h
    Include/Rocket/Core/PropertySpecification.h
    Include/Rocket/Core/ReferenceCountable.h
    Include/Rocket/Core/RenderInterface.h
    Include/Rocket/Core/ScriptInterface.h
    Include/Rocket/Core/Stream.h
    Include/Rocket/Core/StreamMemory.h
    Include/Rocket/Core/String.h
    Include/Rocket/Core/StringBase.h
    Include/Rocket/Core/StringBase.inl
    Include/Rocket/Core/StringUtilities.h
    Include/Rocket/Core/StyleSheet.h
    Include/Rocket/Core/StyleSheetKeywords.h
    Include/Rocket/Core/StyleSheetSpecification.h
    Include/Rocket/Core/SystemInterface.h
    Include/Rocket/Core/Texture.h
    Include/Rocket/Core/TypeConverter.h
    Include/Rocket/Core/TypeConverter.inl
    Include/Rocket/Core/Types.h
    Include/Rocket/Core/URL.h
    Include/Rocket/Core/Variant.h
    Include/Rocket/Core/Variant.inl
    Include/Rocket/Core/Vector2.h
    Include/Rocket/Core/Vector2.inl
    Include/Rocket/Core/Vertex.h
    Include/Rocket/Core/WString.h
    Include/Rocket/Core/XMLNodeHandler.h
    Include/Rocket/Core/XMLParser.h
    Include/Rocket/Core.h
    Include/Rocket/Debugger
    Include/Rocket/Debugger/Debugger.h
    Include/Rocket/Debugger.h
    Source/Controls/Clipboard.cpp
    Source/Controls/Controls.cpp
    Source/Controls/DataFormatter.cpp
    Source/Controls/DataQuery.cpp
    Source/Controls/DataSource.cpp
    Source/Controls/DataSourceListener.cpp
    Source/Controls/ElementDataGrid.cpp
    Source/Controls/ElementDataGridCell.cpp
    Source/Controls/ElementDataGridExpandButton.cpp
    Source/Controls/ElementDataGridRow.cpp
    Source/Controls/ElementForm.cpp
    Source/Controls/ElementFormControl.cpp
    Source/Controls/ElementFormControlDataSelect.cpp
    Source/Controls/ElementFormControlInput.cpp
    Source/Controls/ElementFormControlSelect.cpp
    Source/Controls/ElementFormControlTextArea.cpp
    Source/Controls/ElementTabSet.cpp
    Source/Controls/ElementTextSelection.cpp
    Source/Controls/ElementTextSelection.h
    Source/Controls/InputType.cpp
    Source/Controls/InputType.h
    Source/Controls/InputTypeButton.cpp
    Source/Controls/InputTypeButton.h
    Source/Controls/InputTypeCheckbox.cpp
    Source/Controls/InputTypeCheckbox.h
    Source/Controls/InputTypeRadio.cpp
    Source/Controls/InputTypeRadio.h
    Source/Controls/InputTypeRange.cpp
    Source/Controls/InputTypeRange.h
    Source/Controls/InputTypeSubmit.cpp
    Source/Controls/InputTypeSubmit.h
    Source/Controls/InputTypeText.cpp
    Source/Controls/InputTypeText.h
    Source/Controls/SelectOption.cpp
    Source/Controls/WidgetDropDown.cpp
    Source/Controls/WidgetDropDown.h
    Source/Controls/WidgetSlider.cpp
    Source/Controls/WidgetSlider.h
    Source/Controls/WidgetSliderInput.cpp
    Source/Controls/WidgetSliderInput.h
    Source/Controls/WidgetTextInput.cpp
    Source/Controls/WidgetTextInput.h
    Source/Controls/WidgetTextInputMultiLine.cpp
    Source/Controls/WidgetTextInputMultiLine.h
    Source/Controls/WidgetTextInputSingleLine.cpp
    Source/Controls/WidgetTextInputSingleLine.h
    Source/Controls/WidgetTextInputSingleLinePassword.cpp
    Source/Controls/WidgetTextInputSingleLinePassword.h
    Source/Controls/XMLNodeHandlerDataGrid.cpp
    Source/Controls/XMLNodeHandlerDataGrid.h
    Source/Controls/XMLNodeHandlerTabSet.cpp
    Source/Controls/XMLNodeHandlerTabSet.h
    Source/Controls/XMLNodeHandlerTextArea.cpp
    Source/Controls/XMLNodeHandlerTextArea.h
    Source/Core/BaseXMLParser.cpp
    Source/Core/BitmapFont/BitmapFontDefinitions.h
    Source/Core/BitmapFont/FontFace.cpp
    Source/Core/BitmapFont/FontFace.h
    Source/Core/BitmapFont/FontFaceHandle.cpp
    Source/Core/BitmapFont/FontFaceHandle.h
    Source/Core/BitmapFont/FontFaceLayer.cpp
    Source/Core/BitmapFont/FontFaceLayer.h
    Source/Core/BitmapFont/FontFamily.cpp
    Source/Core/BitmapFont/FontFamily.h
    Source/Core/BitmapFont/FontParser.cpp
    Source/Core/BitmapFont/FontParser.h
    Source/Core/BitmapFont/FontProvider.cpp
    Source/Core/TTFFont/TTFFontDefinitions.h
    Source/Core/TTFFont/FontFace.cpp
    Source/Core/TTFFont/FontFace.h
    Source/Core/TTFFont/FontFaceHandle.cpp
    Source/Core/TTFFont/FontFaceHandle.h
    Source/Core/TTFFont/FontFaceLayer.cpp
    Source/Core/TTFFont/FontFaceLayer.h
    Source/Core/TTFFont/FontFamily.cpp
    Source/Core/TTFFont/FontFamily.h
    Source/Core/TTFFont/FontProvider.cpp
    Source/Core/Box.cpp
    Source/Core/Clock.cpp
    Source/Core/Clock.h
    Source/Core/Context.cpp
    Source/Core/DocumentContext.cpp
    Source/Core/DocumentContextInstancer.cpp
    Source/Core/DocumentContextInstancerDefault.cpp
    Source/Core/DocumentContextInstancerDefault.h
    Source/Core/ConvolutionFilter.cpp
    Source/Core/Core.cpp
    Source/Core/DebugFont.h
    Source/Core/Decorator.cpp
    Source/Core/DecoratorInstancer.cpp
    Source/Core/DecoratorNone.cpp
    Source/Core/DecoratorNone.h
    Source/Core/DecoratorNoneInstancer.cpp
    Source/Core/DecoratorNoneInstancer.h
    Source/Core/DecoratorTiled.cpp
    Source/Core/DecoratorTiled.h
    Source/Core/DecoratorTiledBox.cpp
    Source/Core/DecoratorTiledBox.h
    Source/Core/DecoratorTiledBoxInstancer.cpp
    Source/Core/DecoratorTiledBoxInstancer.h
    Source/Core/DecoratorTiledHorizontal.cpp
    Source/Core/DecoratorTiledHorizontal.h
    Source/Core/DecoratorTiledHorizontalInstancer.cpp
    Source/Core/DecoratorTiledHorizontalInstancer.h
    Source/Core/DecoratorTiledImage.cpp
    Source/Core/DecoratorTiledImage.h
    Source/Core/DecoratorTiledImageInstancer.cpp
    Source/Core/DecoratorTiledImageInstancer.h
    Source/Core/DecoratorTiledInstancer.cpp
    Source/Core/DecoratorTiledInstancer.h
    Source/Core/DecoratorTiledVertical.cpp
    Source/Core/DecoratorTiledVertical.h
    Source/Core/DecoratorTiledVerticalInstancer.cpp
    Source/Core/DecoratorTiledVerticalInstancer.h
    Source/Core/Dictionary.cpp
    Source/Core/DocumentHeader.cpp
    Source/Core/DocumentHeader.h
    Source/Core/Element.cpp
    Source/Core/ElementBackground.cpp
    Source/Core/ElementBackground.h
    Source/Core/ElementBorder.cpp
    Source/Core/ElementBorder.h
    Source/Core/ElementDecoration.cpp
    Source/Core/ElementDecoration.h
    Source/Core/ElementDefinition.cpp
    Source/Core/ElementDefinition.h
    Source/Core/ElementDocument.cpp
    Source/Core/ElementHandle.cpp
    Source/Core/ElementHandle.h
    Source/Core/ElementImage.cpp
    Source/Core/ElementImage.h
    Source/Core/ElementInstancer.cpp
    Source/Core/ElementReference.cpp
    Source/Core/ElementScroll.cpp
    Source/Core/ElementStyle.cpp
    Source/Core/ElementStyle.h
    Source/Core/ElementStyleCache.cpp
    Source/Core/ElementStyleCache.h
    Source/Core/ElementText.cpp
    Source/Core/ElementTextDefault.cpp
    Source/Core/ElementTextDefault.h
    Source/Core/ElementUtilities.cpp
    Source/Core/Event.cpp
    Source/Core/EventDispatcher.cpp
    Source/Core/EventDispatcher.h
    Source/Core/EventInstancer.cpp
    Source/Core/EventInstancerDefault.cpp
    Source/Core/EventInstancerDefault.h
    Source/Core/EventIterators.h
    Source/Core/EventListenerInstancer.cpp
    Source/Core/Factory.cpp
    Source/Core/FileInterface.cpp
    Source/Core/FontDatabase.cpp
    Source/Core/FontEffect.cpp
    Source/Core/FontEffectInstancer.cpp
    Source/Core/FontEffectNone.cpp
    Source/Core/FontEffectNone.h
    Source/Core/FontEffectNoneInstancer.cpp
    Source/Core/FontEffectNoneInstancer.h
    Source/Core/FontEffectOutline.cpp
    Source/Core/FontEffectOutline.h
    Source/Core/FontEffectOutlineInstancer.cpp
    Source/Core/FontEffectOutlineInstancer.h
    Source/Core/FontEffectShadow.cpp
    Source/Core/FontEffectShadow.h
    Source/Core/FontEffectShadowInstancer.cpp
    Source/Core/FontEffectShadowInstancer.h
    Source/Core/FontFace.cpp
    Source/Core/FontFaceHandle.cpp
    Source/Core/FontFaceHandle.h
    Source/Core/FontFaceLayer.cpp
    Source/Core/FontFaceLayer.h
    Source/Core/FontFamily.cpp
    Source/Core/FontProvider.cpp
    Source/Core/Geometry.cpp
    Source/Core/GeometryDatabase.cpp
    Source/Core/GeometryDatabase.h
    Source/Core/GeometryUtilities.cpp
    Source/Core/LayoutBlockBox.cpp
    Source/Core/LayoutBlockBox.h
    Source/Core/LayoutBlockBoxSpace.cpp
    Source/Core/LayoutBlockBoxSpace.h
    Source/Core/LayoutEngine.cpp
    Source/Core/LayoutEngine.h
    Source/Core/LayoutInlineBox.cpp
    Source/Core/LayoutInlineBox.h
    Source/Core/LayoutInlineBoxText.cpp
    Source/Core/LayoutInlineBoxText.h
    Source/Core/LayoutLineBox.cpp
    Source/Core/LayoutLineBox.h
    Source/Core/Log.cpp
    Source/Core/Math.cpp
    Source/Core/Plugin.cpp
    Source/Core/PluginRegistry.cpp
    Source/Core/PluginRegistry.h
    Source/Core/Pool.h
    Source/Core/Pool.inl
    Source/Core/precompiled.h
    Source/Core/Property.cpp
    Source/Core/PropertyDefinition.cpp
    Source/Core/PropertyDictionary.cpp
    Source/Core/PropertyParserColour.cpp
    Source/Core/PropertyParserColour.h
    Source/Core/PropertyParserKeyword.cpp
    Source/Core/PropertyParserKeyword.h
    Source/Core/PropertyParserNumber.cpp
    Source/Core/PropertyParserNumber.h
    Source/Core/PropertyParserString.cpp
    Source/Core/PropertyParserString.h
    Source/Core/PropertyShorthandDefinition.h
    Source/Core/PropertySpecification.cpp
    Source/Core/ReferenceCountable.cpp
    Source/Core/RenderInterface.cpp
    Source/Core/Stream.cpp
    Source/Core/StreamFile.cpp
    Source/Core/StreamFile.h
    Source/Core/StreamMemory.cpp
    Source/Core/String.cpp
    Source/Core/StringCache.cpp
    Source/Core/StringCache.h
    Source/Core/StringUtilities.cpp
    Source/Core/StyleSheet.cpp
    Source/Core/StyleSheetFactory.cpp
    Source/Core/StyleSheetFactory.h
    Source/Core/StyleSheetNode.cpp
    Source/Core/StyleSheetNode.h
    Source/Core/StyleSheetNodeSelector.cpp
    Source/Core/StyleSheetNodeSelector.h
    Source/Core/StyleSheetNodeSelectorEmpty.cpp
    Source/Core/StyleSheetNodeSelectorEmpty.h
    Source/Core/StyleSheetNodeSelectorFirstChild.cpp
    Source/Core/StyleSheetNodeSelectorFirstChild.h
    Source/Core/StyleSheetNodeSelectorFirstOfType.cpp
    Source/Core/StyleSheetNodeSelectorFirstOfType.h
    Source/Core/StyleSheetNodeSelectorLastChild.cpp
    Source/Core/StyleSheetNodeSelectorLastChild.h
    Source/Core/StyleSheetNodeSelectorLastOfType.cpp
    Source/Core/StyleSheetNodeSelectorLastOfType.h
    Source/Core/StyleSheetNodeSelectorNthChild.cpp
    Source/Core/StyleSheetNodeSelectorNthChild.h
    Source/Core/StyleSheetNodeSelectorNthLastChild.cpp
    Source/Core/StyleSheetNodeSelectorNthLastChild.h
    Source/Core/StyleSheetNodeSelectorNthLastOfType.cpp
    Source/Core/StyleSheetNodeSelectorNthLastOfType.h
    Source/Core/StyleSheetNodeSelectorNthOfType.cpp
    Source/Core/StyleSheetNodeSelectorNthOfType.h
    Source/Core/StyleSheetNodeSelectorOnlyChild.cpp
    Source/Core/StyleSheetNodeSelectorOnlyChild.h
    Source/Core/StyleSheetNodeSelectorOnlyOfType.cpp
    Source/Core/StyleSheetNodeSelectorOnlyOfType.h
    Source/Core/StyleSheetParser.cpp
    Source/Core/StyleSheetParser.h
    Source/Core/StyleSheetSpecification.cpp
    Source/Core/SystemInterface.cpp
    Source/Core/Template.cpp
    Source/Core/Template.h
    Source/Core/TemplateCache.cpp
    Source/Core/TemplateCache.h
    Source/Core/Texture.cpp
    Source/Core/TextureDatabase.cpp
    Source/Core/TextureDatabase.h
    Source/Core/TextureLayout.cpp
    Source/Core/TextureLayout.h
    Source/Core/TextureLayoutRectangle.cpp
    Source/Core/TextureLayoutRectangle.h
    Source/Core/TextureLayoutRow.cpp
    Source/Core/TextureLayoutRow.h
    Source/Core/TextureLayoutTexture.cpp
    Source/Core/TextureLayoutTexture.h
    Source/Core/TextureResource.cpp
    Source/Core/TextureResource.h
    Source/Core/UnicodeRange.cpp
    Source/Core/UnicodeRange.h
    Source/Core/URL.cpp
    Source/Core/Variant.cpp
    Source/Core/WidgetSlider.cpp
    Source/Core/WidgetSlider.h
    Source/Core/WidgetSliderScroll.cpp
    Source/Core/WidgetSliderScroll.h
    Source/Core/WString.cpp
    Source/Core/XMLNodeHandler.cpp
    Source/Core/XMLNodeHandlerBody.cpp
    Source/Core/XMLNodeHandlerBody.h
    Source/Core/XMLNodeHandlerDefault.cpp
    Source/Core/XMLNodeHandlerDefault.h
    Source/Core/XMLNodeHandlerHead.cpp
    Source/Core/XMLNodeHandlerHead.h
    Source/Core/XMLNodeHandlerTemplate.cpp
    Source/Core/XMLNodeHandlerTemplate.h
    Source/Core/XMLParser.cpp
    Source/Core/XMLParseTools.cpp
    Source/Core/XMLParseTools.h
    Source/Debugger/BeaconSource.h
    Source/Debugger/CommonSource.h
    Source/Debugger/Debugger.cpp
    Source/Debugger/ElementContextHook.cpp
    Source/Debugger/ElementContextHook.h
    Source/Debugger/ElementInfo.cpp
    Source/Debugger/ElementInfo.h
    Source/Debugger/ElementLog.cpp
    Source/Debugger/ElementLog.h
    Source/Debugger/FontSource.h
    Source/Debugger/Geometry.cpp
    Source/Debugger/Geometry.h
    Source/Debugger/InfoSource.h
    Source/Debugger/LogSource.h
    Source/Debugger/MenuSource.h
    Source/Debugger/Plugin.cpp
    Source/Debugger/Plugin.h
    Source/Debugger/SystemInterface.cpp
    Source/Debugger/SystemInterface.h
  LIBS
    stb
)

post_enable_overlay_directory()
