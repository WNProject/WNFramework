// Copyright (c) 2021, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/inc/ui_scripting_parser.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "librocket/Source/Core/DecoratorTiledImageInstancer.h"

namespace wn {
namespace engine {
namespace ui {

namespace {
wn::containers::string_view get_string_view(
    const Rocket::Core::String& string) {
  return wn::containers::string_view(string.CString());
}
}  // namespace

class dummy_decorator : public Rocket::Core::Decorator {
public:
  dummy_decorator(Rocket::Core::Context* _context)
    : Rocket::Core::Decorator(_context) {}

  Rocket::Core::DecoratorDataHandle GenerateElementData(
      Rocket::Core::Element*) override {
    return 1;
  }
  void ReleaseElementData(Rocket::Core::DecoratorDataHandle) override {
    return;
  }

  void RenderElement(
      Rocket::Core::Element*, Rocket::Core::DecoratorDataHandle) override {
    return;
  }
  void Release() {}
};

class dummy_tiled_image_instancer
  : public Rocket::Core::DecoratorTiledImageInstancer {
public:
  dummy_tiled_image_instancer(Rocket::Core::Context* _context,
      memory::allocator* _allocator,
      containers::dynamic_array<containers::string>* _includes)
    : Rocket::Core::DecoratorTiledImageInstancer(_context),
      m_allocator(_allocator),
      m_includes(_includes) {}

  Rocket::Core::Decorator* InstanceDecorator(
      const Rocket::Core::String& ROCKET_UNUSED_PARAMETER(name),
      const Rocket::Core::PropertyDictionary& _properties) override {
    Rocket::Core::DecoratorTiled::Tile tile;
    Rocket::Core::String texture_name;
    Rocket::Core::String rcss_path;
    GetTileProperties(tile, texture_name, rcss_path, _properties, "image");
    containers::string_view resource_name(
        texture_name.CString(), texture_name.Length());
    if (!resource_name.starts_with("@Texture(\"")) {
      return nullptr;
    }
    resource_name = resource_name.substr(10);
    size_t ep = resource_name.find_first_of('"');
    if (ep == containers::string_view::npos) {
      return nullptr;
    }
    resource_name = resource_name.substr(0, ep);
    m_includes->push_back(resource_name.to_string(m_allocator));
    return new dummy_decorator(m_context);
  }

  void ReleaseDecorator(Rocket::Core::Decorator* _decorator) override {
    delete _decorator;
  }
  // Releases the instancer.
  void Release() override {}
  memory::allocator* m_allocator;
  containers::dynamic_array<containers::string>* m_includes;
};
// TODO(awoloszyn): Extend this with instancers for other things that take
// images

// Some dummy interfaces so that we can actually bring up Rocket
class dummy_system_interface : public Rocket::Core::SystemInterface {
public:
  dummy_system_interface(logging::log* _log) : m_log(_log) {}
  bool LogMessage(Rocket::Core::Log::Type type,
      const Rocket::Core::String& message) override {
    switch (type) {
      case Rocket::Core::Log::LT_ALWAYS:
        m_log->log_critical(message.CString());
        break;
      case Rocket::Core::Log::LT_ASSERT:
        m_log->log_critical("ROCKET_ASSERT: ", message.CString());
        break;
      case Rocket::Core::Log::LT_DEBUG:
        m_log->log_debug(message.CString());
        break;
      case Rocket::Core::Log::LT_ERROR:
        m_log->log_error(message.CString());
        break;
      case Rocket::Core::Log::LT_INFO:
        m_log->log_info(message.CString());
        break;
      case Rocket::Core::Log::LT_WARNING:
        m_log->log_warning(message.CString());
        break;
      case Rocket::Core::Log::LT_MAX:
        m_log->log_critical(message.CString());
        break;
    }
    return true;
  }

  float GetElapsedTime() override {
    return 0.0f;
  }

private:
  logging::log* m_log;
};

class base_document : public Rocket::Core::ElementDocument {
public:
  ROCKET_RTTI_DefineWithParent(Rocket::Core::ElementDocument);

  base_document(memory::allocator* _allocator,
      containers::dynamic_array<containers::string>* _includes,
      containers::string* _out_string, Rocket::Core::Context* _ctx,
      const Rocket::Core::String& tag)
    : Rocket::Core::ElementDocument(_ctx, tag),
      m_allocator(_allocator),
      m_includes(_includes),
      m_out_string(_out_string) {}

  void LoadScript(Rocket::Core::Stream* stream,
      const Rocket::Core::String& source_name) override;

  memory::allocator* m_allocator;
  containers::dynamic_array<containers::string>* m_includes;
  containers::string* m_out_string;
};

class base_document_instancer : public Rocket::Core::ElementInstancer {
public:
  base_document_instancer(memory::allocator* _allocator,
      containers::dynamic_array<containers::string>* _includes,
      containers::string* _out_string)
    : m_allocator(_allocator),
      m_includes(_includes),
      m_out_string(_out_string) {}

  Rocket::Core::Element* InstanceElement(Rocket::Core::Context* _context,
      Rocket::Core::Element* parent, const Rocket::Core::String& tag,
      const Rocket::Core::XMLAttributes&) override {
    // TODO(awoloszyn) update ALL of this, we should really
    // be using an allocator always;
    (void)parent;
    return new base_document(
        m_allocator, m_includes, m_out_string, _context, tag);
  }

  void ReleaseElement(Rocket::Core::Element* element) override {
    delete element;
  }

  // Not deleting because this should only exist on the stack.
  void Release() override {}

private:
  memory::allocator* m_allocator;
  containers::dynamic_array<containers::string>* m_includes;
  containers::string* m_out_string;
};

class dummy_file_interface : public Rocket::Core::FileInterface {
  struct dummy_file {
    wn::file_system::file_ptr m_file;
    size_t m_offset;
  };

public:
  dummy_file_interface(file_system::mapping* _mapping) : m_mapping(_mapping) {}

  Rocket::Core::FileHandle Open(const Rocket::Core::String& _path) override {
    wn::file_system::result res = wn::file_system::result::ok;
    wn::file_system::file_ptr file =
        m_mapping->open_file(get_string_view(_path), res);
    if (res != wn::file_system::result::ok) {
      return reinterpret_cast<Rocket::Core::FileHandle>(nullptr);
    }
    // TODO(awoloszyn): At least allocate this with the allocator
    dummy_file* f = new dummy_file();
    f->m_offset = 0;
    f->m_file = wn::core::move(file);
    return reinterpret_cast<Rocket::Core::FileHandle>(f);
  }

  void Close(Rocket::Core::FileHandle _file) override {
    dummy_file* f = reinterpret_cast<dummy_file*>(_file);
    delete f;
  }

  size_t Read(
      void* _buffer, size_t _size, Rocket::Core::FileHandle _file) override {
    dummy_file* f = reinterpret_cast<dummy_file*>(_file);

    size_t to_read = wn::math::min(_size, f->m_file->size() - f->m_offset);
    wn::memory::memcpy(_buffer, f->m_file->data() + f->m_offset, to_read);
    f->m_offset += to_read;
    return to_read;
  }
  bool Seek(
      Rocket::Core::FileHandle _file, long _offset, int _origin) override {
    dummy_file* f = reinterpret_cast<dummy_file*>(_file);
    WN_RELEASE_ASSERT(_offset >= 0, "_offset must be positive");
    size_t offs = static_cast<size_t>(_offset);
    switch (_origin) {
      case SEEK_SET:
        if (offs > f->m_file->size()) {
          return false;
        }
        f->m_offset = offs;
        break;
      case SEEK_CUR:
        if (offs + f->m_offset > f->m_file->size()) {
          return false;
        }
        f->m_offset += offs;
        break;
      case SEEK_END:
        if (offs > f->m_file->size()) {
          return false;
        }
        f->m_offset = f->m_file->size() - offs;
        break;
    }
    return true;
  }
  size_t Tell(Rocket::Core::FileHandle _file) override {
    dummy_file* f = reinterpret_cast<dummy_file*>(_file);
    return f->m_offset;
  }

private:
  file_system::mapping* m_mapping;
};

class dummy_renderer : public Rocket::Core::RenderInterface {
public:
  dummy_renderer(Rocket::Core::Context* _context)
    : Rocket::Core::RenderInterface(_context) {}
  void RenderGeometry(Rocket::Core::Vertex*, int, int*, int,
      Rocket::Core::TextureHandle, const Rocket::Core::Vector2f&) override {}
  void EnableScissorRegion(bool) override {}
  void SetScissorRegion(int, int, int, int) override {}
  bool LoadTexture(Rocket::Core::TextureHandle&, Rocket::Core::Vector2i&,
      const Rocket::Core::String&) override {
    return false;
  }
  bool GenerateTexture(Rocket::Core::TextureHandle&, const Rocket::Core::byte*,
      const Rocket::Core::Vector2i&) override {
    return false;
  }
  void ReleaseTexture(Rocket::Core::TextureHandle) override {}
  Rocket::Core::CompiledGeometryHandle CompileGeometry(Rocket::Core::Vertex*,
      int, int*, int, Rocket::Core::TextureHandle) override {
    return 0;
  }
  void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle,
      const Rocket::Core::Vector2f&) override {}
  void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle) override {}
  float GetPixelsPerInch() override {
    return 1.0f;
  }
};

void base_document::LoadScript(
    Rocket::Core::Stream* stream, const Rocket::Core::String& source_name) {
  if (source_name != "") {
    m_includes->push_back(
        containers::string(m_allocator, source_name.CString()));
    return;
  }
  char bytes[1024];
  size_t n_read = 0;
  while (n_read = stream->Read(bytes, 1024), n_read) {
    m_out_string->append(bytes, n_read);
  }
}

bool ui_scripting_parser::parse_ui(file_system::mapping* _mapping,
    containers::string_view _file_name, containers::string* _out_string,
    containers::string* _out_ui_name) {
  containers::dynamic_array<containers::string> includes(m_allocator);
  containers::string out_string(m_allocator);

  Rocket::Core::Context context;
  context.SetLoadScriptContents(false);
  dummy_renderer renderer(&context);
  dummy_system_interface system_interface(m_log);
  dummy_file_interface file_interface(_mapping);

  Rocket::Core::SetRenderInterface(&context, &renderer);
  Rocket::Core::SetSystemInterface(&context, &system_interface);
  Rocket::Core::SetFileInterface(&context, &file_interface);

  Rocket::Core::Initialise(&context);
  base_document_instancer base_instancer(m_allocator, &includes, &out_string);
  Rocket::Core::Factory::RegisterElementInstancer(
      &context, "body", &base_instancer)
      ->RemoveReference();

  dummy_tiled_image_instancer image_instancer(&context, m_allocator, &includes);
  Rocket::Core::Factory::RegisterDecoratorInstancer(
      &context, "image", &image_instancer)
      ->RemoveReference();

  Rocket::Core::DocumentContext* documents =
      Rocket::Core::CreateDocumentContext(
          &context, "main", Rocket::Core::Vector2i(1024, 1024));
  containers::string str(m_allocator, _file_name);
  Rocket::Core::ElementDocument* document =
      documents->LoadDocument(str.c_str());

  if (document) {
    document->RemoveReference();
  }
  documents->UnloadAllDocuments();
  documents->RemoveReference();
  Rocket::Core::Shutdown(&context);
  m_log->flush();
  if (document == nullptr) {
    return false;
  }

  size_t ui_name_pos = out_string.find("// UIName=");

  if (ui_name_pos != containers::string::npos) {
    ui_name_pos += 10;  // strlen(// UIName=)
    size_t end = out_string.find_first_of("\n\r", ui_name_pos);
    *_out_ui_name =
        out_string.to_string_view().substr(ui_name_pos, end - ui_name_pos);
  } else {
    m_log->log_error(_file_name, ": Missing in script tag \"// UIName=\"");
    return false;
  }

  for (auto inc : includes) {
    *_out_string += "include \"";
    *_out_string += inc.c_str();
    *_out_string += "\"\n";
  }
  *_out_string += "include \"ui/renderable_ui.wns\"\n\n";
  *_out_string += "class ";
  *_out_string += *_out_ui_name;
  *_out_string += " : RenderableUI {\n";
  *_out_string += " .name = \"";
  *_out_string += _file_name;
  *_out_string += "\";\n";
  *_out_string += out_string;
  *_out_string += "\n}\n\n shared ";

  *_out_string += *_out_ui_name;
  *_out_string += " getNew" + *_out_ui_name + "() { shared " + *_out_ui_name +
                  " x = shared " + *_out_ui_name + "(); return x; }\n";

  return document != nullptr;
}

}  // namespace ui
}  // namespace engine
}  // namespace wn
