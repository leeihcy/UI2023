#ifndef _UI_SDK_SRC_RENDER_IMAGE_RENDER_IMAGE_RENDER_META_H_
#define _UI_SDK_SRC_RENDER_IMAGE_RENDER_IMAGE_RENDER_META_H_
#include "include/interface/imeta.h"
#include "include/interface/irenderbase.h"
#include "include/interface/icolorrender.h"
#include "include/interface/iimagerender.h"
#include "include/interface/itextrenderbase.h"
#include "include/macro/xmldefine.h"

namespace ui {
struct IColorRender;
struct IColorListRender;
struct ISysColorRender;
struct IGradientRender;
struct IImageRender;
struct IImageListRender;
struct IImageListItemRender;


struct RenderBaseMeta : public MetaImpl<IRenderBase> {
  static RenderBaseMeta& Get() {static RenderBaseMeta s; return s; }

  Uuid UUID() override { return {0xBEB29366, 0x3E70, 0x11EE, {0x91, 0x01, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct ColorRenderMeta : public MetaImpl<IColorRender> {
  static ColorRenderMeta& Get() {static ColorRenderMeta s; return s; }

  Uuid UUID() override { return {0x1D7141D8, 0x3E71, 0x11EE, {0x83, 0xB7, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return XML_RENDER_TYPE_COLOR; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = RENDER;
    param.minor_type = RENDER_TYPE_COLOR;
    return param;
  }
}; 

struct ImageRenderMeta : public MetaImpl<IImageRender> {
  static ImageRenderMeta& Get() {static ImageRenderMeta s; return s; }

  Uuid UUID() override { return {0xD0F5EAEC, 0x3E6F, 0x11EE, {0xA1, 0xC2, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return XML_RENDER_TYPE_IMAGE; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = RENDER;
    param.minor_type = RENDER_TYPE_IMAGE;
    return param;
  }
};

struct ImageListRenderMeta : public MetaImpl<IImageListRender> {
  static ImageListRenderMeta& Get() {static ImageListRenderMeta s; return s; }

  Uuid UUID() override { return {0xF1921FDA, 0x3E71, 0x11EE, {0xB2, 0x11, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return XML_RENDER_TYPE_IMAGELIST; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = RENDER;
    param.minor_type = RENDER_TYPE_IMAGELIST;
    return param;
  }
};

struct ImageListItemRenderMeta : public MetaImpl<IImageListItemRender> {
  static ImageListItemRenderMeta& Get() {static ImageListItemRenderMeta s; return s; }

  Uuid UUID() override { return {0xDFC9A03E, 0x3E71, 0x11EE, {0x88, 0x1D, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const char* Name() override { return XML_RENDER_TYPE_IMAGELISTITEM; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = RENDER;
    param.minor_type = RENDER_TYPE_IMAGELISTITEM;
    return param;
  }
};

struct TextRenderBaseMeta : public MetaImpl<ITextRenderBase> {
  static TextRenderBaseMeta& Get() {static TextRenderBaseMeta s; return s; }

  Uuid UUID() override { return {0x870C16F4, 0x3E88, 0x11EE, {0xA0, 0x2B, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct SimpleTextRenderMeta : public MetaImpl<ISimpleTextRender> {
  static SimpleTextRenderMeta& Get() {static SimpleTextRenderMeta s; return s; }

  Uuid UUID() override { return {0xA31572D2, 0x3E88, 0x11EE, {0xAB, 0x10, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return XML_TEXTRENDER_TYPE_SIMPLE; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = TEXT_RENDER;
    param.minor_type = TEXTRENDER_TYPE_SIMPLE;
    return param;
  }
};

struct ColorListTextRenderMeta : public MetaImpl<IColorListTextRender> {
  static ColorListTextRenderMeta& Get() {static ColorListTextRenderMeta s; return s; }

  Uuid UUID() override { return {0xF846F9C6, 0x3E88, 0x11EE, {0xBD, 0x07, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return XML_TEXTRENDER_TYPE_COLORLIST; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = TEXT_RENDER;
    param.minor_type = TEXTRENDER_TYPE_COLORLIST;
    return param;
  }
};

#if 0
struct ContrastColorTextRenderMeta : public MetaImpl<IContrastColorTextRender> {
  static ContrastColorTextRenderMeta& Get() {static ContrastColorTextRenderMeta s; return s; }

  Uuid UUID() override { return {0xB8118658, 0x3E88, 0x11EE, {0xBD, 0xBD, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = TEXT_RENDER;
    param.minor_type = TEXTRENDER_TYPE_COLORLIST;
    return param;
  }
};

struct ContrastColorListTextRenderMeta : public MetaImpl<IContrastColorListTextRender> {
  static ContrastColorListTextRenderMeta& Get() {static ContrastColorListTextRenderMeta s; return s; }

  Uuid UUID() override { return {0x55A21DEC, 0x3E89, 0x11EE, {0xA7, 0x63, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = TEXT_RENDER;
    param.minor_type = TEXTRENDER_TYPE_CONTRASTCOLORLIST;
    return param;
  }
};


struct FontColorListTextRenderMeta : public MetaImpl<IFontColorListTextRender> {
  static FontColorListTextRenderMeta& Get() {static FontColorListTextRenderMeta s; return s; }

  Uuid UUID() override { return {0xE7397BAC, 0x3E88, 0x11EE, {0xB8, 0xB5, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const char* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    param.major_type = TEXT_RENDER;
    param.minor_type = TEXTRENDER_TYPE_FONTCOLORLIST;
    return param;
  }
};

#endif

}

#endif