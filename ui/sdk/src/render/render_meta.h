#ifndef _UI_SDK_SRC_RENDER_IMAGE_RENDER_IMAGE_RENDER_META_H_
#define _UI_SDK_SRC_RENDER_IMAGE_RENDER_IMAGE_RENDER_META_H_
#include "include/interface/imeta.h"
#include "include/interface/irenderbase.h"
#include "include/interface/icolorrender.h"
#include "include/interface/iimagerender.h"

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
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct ColorRenderMeta : public MetaImpl<IColorRender> {
  static ColorRenderMeta& Get() {static ColorRenderMeta s; return s; }

  Uuid UUID() override { return {0x1D7141D8, 0x3E71, 0x11EE, {0x83, 0xB7, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct ColorListRenderMeta : public MetaImpl<IColorListRender> {
  static ColorListRenderMeta& Get() {static ColorListRenderMeta s; return s; }

  Uuid UUID() override { return {0x61B7CD08, 0x3E88, 0x11EE, {0x82, 0x98, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct SysColorRenderMeta : public MetaImpl<ISysColorRender> {
  static SysColorRenderMeta& Get() {static SysColorRenderMeta s; return s; }

  Uuid UUID() override { return {0x1978EFC6, 0x3E72, 0x11EE, {0x84, 0x02, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct GradientRenderMeta : public MetaImpl<IGradientRender> {
  static GradientRenderMeta& Get() {static GradientRenderMeta s; return s; }

  Uuid UUID() override { return {0x2EA910E2, 0x3E72, 0x11EE, {0xA5, 0x8B, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};


struct ImageRenderMeta : public MetaImpl<IImageRender> {
  static ImageRenderMeta& Get() {static ImageRenderMeta s; return s; }

  Uuid UUID() override { return {0xD0F5EAEC, 0x3E6F, 0x11EE, {0xA1, 0xC2, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct ImageListRenderMeta : public MetaImpl<IImageListRender> {
  static ImageListRenderMeta& Get() {static ImageListRenderMeta s; return s; }

  Uuid UUID() override { return {0xF1921FDA, 0x3E71, 0x11EE, {0xB2, 0x11, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct ImageListItemRenderMeta : public MetaImpl<IImageListItemRender> {
  static ImageListItemRenderMeta& Get() {static ImageListItemRenderMeta s; return s; }

  Uuid UUID() override { return {0xDFC9A03E, 0x3E71, 0x11EE, {0x88, 0x1D, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }

  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct TextRenderBaseMeta : public MetaImpl<ITextRenderBase> {
  static TextRenderBaseMeta& Get() {static TextRenderBaseMeta s; return s; }

  Uuid UUID() override { return {0x870C16F4, 0x3E88, 0x11EE, {0xA0, 0x2B, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct SimpleTextRenderMeta : public MetaImpl<ISimpleTextRender> {
  static SimpleTextRenderMeta& Get() {static SimpleTextRenderMeta s; return s; }

  Uuid UUID() override { return {0xA31572D2, 0x3E88, 0x11EE, {0xAB, 0x10, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct ContrastColorTextRenderMeta : public MetaImpl<IContrastColorTextRender> {
  static ContrastColorTextRenderMeta& Get() {static ContrastColorTextRenderMeta s; return s; }

  Uuid UUID() override { return {0xB8118658, 0x3E88, 0x11EE, {0xBD, 0xBD, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct ContrastColorListTextRenderMeta : public MetaImpl<IContrastColorListTextRender> {
  static ContrastColorListTextRenderMeta& Get() {static ContrastColorListTextRenderMeta s; return s; }

  Uuid UUID() override { return {0x55A21DEC, 0x3E89, 0x11EE, {0xA7, 0x63, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};


struct FontColorListTextRenderMeta : public MetaImpl<IFontColorListTextRender> {
  static FontColorListTextRenderMeta& Get() {static FontColorListTextRenderMeta s; return s; }

  Uuid UUID() override { return {0xE7397BAC, 0x3E88, 0x11EE, {0xB8, 0xB5, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};

struct ColorListTextRenderMeta : public MetaImpl<IColorListTextRender> {
  static ColorListTextRenderMeta& Get() {static ColorListTextRenderMeta s; return s; }

  Uuid UUID() override { return {0xF846F9C6, 0x3E88, 0x11EE, {0xBD, 0x07, 0xF4, 0x5C, 0x89, 0xB0, 0x17, 0x4F}}; }
  const wchar_t* Name() override { return nullptr; }
  MetaDetail Detail() override {
    MetaDetail param = { 0 };
    return param;
  }
};


}

#endif