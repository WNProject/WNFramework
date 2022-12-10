// Copyright (c) 2022, WNProject Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/inc/ui_scripting_parser.h"
#include "WNFileSystem/inc/WNFactory.h"
#include "WNFileSystem/inc/WNMapping.h"
#include "rmlui/Source/Core/DecoratorTiledImageInstancer.h"

namespace wn {
namespace engine {
namespace ui {

namespace {
wn::containers::string_view get_string_view(const Rml::String& string) {
  return wn::containers::string_view(string.c_str());
}
}  // namespace

class dummy_decorator : public Rml::Decorator {
public:
  dummy_decorator(/*Rml::Context* _context*/) : Rml::Decorator(/*_context*/) {}

  Rml::DecoratorDataHandle GenerateElementData(Rml::Element*) const override {
    return 1;
  }
  void ReleaseElementData(Rml::DecoratorDataHandle) const override {
    return;
  }

  void RenderElement(Rml::Element*, Rml::DecoratorDataHandle) const override {
    return;
  }
};

class dummy_tiled_image_instancer : public Rml::DecoratorTiledImageInstancer {
public:
  dummy_tiled_image_instancer(memory::allocator* _allocator,
      containers::dynamic_array<containers::string>* _includes)
    : Rml::DecoratorTiledImageInstancer(),
      m_allocator(_allocator),
      m_includes(_includes) {}

  Rml::SharedPtr<Rml::Decorator> InstanceDecorator(
      const Rml::String& RMLUI_UNUSED_PARAMETER(name),
      const Rml::PropertyDictionary& _properties,
      const Rml::DecoratorInstancerInterface& instancer_interface) override {
    RMLUI_UNUSED(name);
    Rml::DecoratorTiled::Tile tile;
    Rml::Texture texture;

    GetTileProperties(&tile, &texture, 1, _properties, instancer_interface);
    const auto& texture_name = texture.GetSource();
    containers::string_view resource_name(
        texture_name.c_str(), texture_name.size());
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
    return Rml::MakeShared<dummy_decorator>();
  }

  memory::allocator* m_allocator;
  containers::dynamic_array<containers::string>* m_includes;
};
// TODO(awoloszyn): Extend this with instancers for other things that take
// images

// Some dummy interfaces so that we can actually bring up Rocket
class dummy_system_interface : public Rml::SystemInterface {
public:
  dummy_system_interface(logging::log* _log) : m_log(_log) {}
  bool LogMessage(Rml::Log::Type type, const Rml::String& message) override {
    switch (type) {
      case Rml::Log::LT_ALWAYS:
        m_log->log_critical(message.c_str());
        break;
      case Rml::Log::LT_ASSERT:
        m_log->log_critical("ROCKET_ASSERT: ", message.c_str());
        break;
      case Rml::Log::LT_DEBUG:
        m_log->log_debug(message.c_str());
        break;
      case Rml::Log::LT_ERROR:
        m_log->log_error(message.c_str());
        break;
      case Rml::Log::LT_INFO:
        m_log->log_info(message.c_str());
        break;
      case Rml::Log::LT_WARNING:
        m_log->log_warning(message.c_str());
        break;
      case Rml::Log::LT_MAX:
        m_log->log_critical(message.c_str());
        break;
    }
    return true;
  }

  double GetElapsedTime() override {
    return 0.0f;
  }

private:
  logging::log* m_log;
};

class base_document : public Rml::ElementDocument {
public:
  RMLUI_RTTI_DefineWithParent(base_document, Rml::ElementDocument);

  base_document(memory::allocator* _allocator,
      containers::dynamic_array<containers::string>* _includes,
      containers::string* _out_string, const Rml::String& tag)
    : Rml::ElementDocument(tag),
      m_allocator(_allocator),
      m_includes(_includes),
      m_out_string(_out_string) {}

  void LoadInlineScript(const Rml::String& content,
      const Rml::String& source_path, int source_line) override;

  void LoadExternalScript(const Rml::String& source_path) override;

  memory::allocator* m_allocator;
  containers::dynamic_array<containers::string>* m_includes;
  containers::string* m_out_string;
};

class base_document_instancer : public Rml::ElementInstancer {
public:
  base_document_instancer(memory::allocator* _allocator,
      containers::dynamic_array<containers::string>* _includes,
      containers::string* _out_string)
    : m_allocator(_allocator),
      m_includes(_includes),
      m_out_string(_out_string) {}

  Rml::ElementPtr InstanceElement(Rml::Element*, const Rml::String& tag,
      const Rml::XMLAttributes&) override {
    // TODO(awoloszyn) update ALL of this, we should really
    // be using an allocator always;

    return Rml::ElementPtr(
        new base_document(m_allocator, m_includes, m_out_string, tag));
  }

  void ReleaseElement(Rml::Element* element) override {
    delete element;
  }

private:
  memory::allocator* m_allocator;
  containers::dynamic_array<containers::string>* m_includes;
  containers::string* m_out_string;
};

class dummy_file_interface : public Rml::FileInterface {
  struct dummy_file {
    wn::file_system::file_ptr m_file;
    size_t m_offset;
  };

public:
  dummy_file_interface(file_system::mapping* _mapping) : m_mapping(_mapping) {}

  Rml::FileHandle Open(const Rml::String& _path) override {
    wn::file_system::result res = wn::file_system::result::ok;
    wn::file_system::file_ptr file =
        m_mapping->open_file(get_string_view(_path), res);
    if (res != wn::file_system::result::ok) {
      return reinterpret_cast<Rml::FileHandle>(nullptr);
    }
    // TODO(awoloszyn): At least allocate this with the allocator
    dummy_file* f = new dummy_file();
    f->m_offset = 0;
    f->m_file = wn::core::move(file);
    return reinterpret_cast<Rml::FileHandle>(f);
  }

  void Close(Rml::FileHandle _file) override {
    dummy_file* f = reinterpret_cast<dummy_file*>(_file);
    delete f;
  }

  size_t Read(void* _buffer, size_t _size, Rml::FileHandle _file) override {
    dummy_file* f = reinterpret_cast<dummy_file*>(_file);

    size_t to_read = wn::math::min(_size, f->m_file->size() - f->m_offset);
    wn::memory::memcpy(_buffer, f->m_file->data() + f->m_offset, to_read);
    f->m_offset += to_read;
    return to_read;
  }
  bool Seek(Rml::FileHandle _file, long _offset, int _origin) override {
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
  size_t Tell(Rml::FileHandle _file) override {
    dummy_file* f = reinterpret_cast<dummy_file*>(_file);
    return f->m_offset;
  }

private:
  file_system::mapping* m_mapping;
};

class dummy_renderer : public Rml::RenderInterface {
public:
  dummy_renderer() : Rml::RenderInterface() {}
  void RenderGeometry(Rml::Vertex*, int, int*, int, Rml::TextureHandle,
      const Rml::Vector2f&) override {}
  void EnableScissorRegion(bool) override {}
  void SetScissorRegion(int, int, int, int) override {}
  bool LoadTexture(
      Rml::TextureHandle&, Rml::Vector2i&, const Rml::String&) override {
    return false;
  }
  bool GenerateTexture(
      Rml::TextureHandle&, const Rml::byte*, const Rml::Vector2i&) override {
    return false;
  }
  void ReleaseTexture(Rml::TextureHandle) override {}
  Rml::CompiledGeometryHandle CompileGeometry(
      Rml::Vertex*, int, int*, int, Rml::TextureHandle) override {
    return 0;
  }
  void RenderCompiledGeometry(
      Rml::CompiledGeometryHandle, const Rml::Vector2f&) override {}
  void ReleaseCompiledGeometry(Rml::CompiledGeometryHandle) override {}
};

// void base_document::LoadScript(
//     Rml::Stream* stream, const Rml::String& source_name) {
//   if (source_name != "") {
//     m_includes->push_back(containers::string(m_allocator,
//     source_name.c_str())); return;
//   }
//   char bytes[1024];
//   size_t n_read = 0;
//   while (n_read = stream->Read(bytes, 1024), n_read) {
//     m_out_string->append(bytes, n_read);
//   }
// }
void base_document::LoadInlineScript(const Rml::String& content,
    const Rml::String& RMLUI_UNUSED_PARAMETER(source_path),
    int RMLUI_UNUSED_PARAMETER(source_line)) {
  RMLUI_UNUSED(source_path);
  RMLUI_UNUSED(source_line);
  m_out_string->append(content.data(), content.size());
}

void base_document::LoadExternalScript(const Rml::String& source_path) {
  m_includes->push_back(containers::string(m_allocator, source_path.c_str()));
}

bool ui_scripting_parser::parse_ui(file_system::mapping* _mapping,
    containers::string_view _file_name, containers::string* _out_string,
    containers::string* _out_ui_name) {
  containers::dynamic_array<containers::string> includes(m_allocator);
  containers::string out_string(m_allocator);

  Rml::GlobalContext ctx;
  Rml::SetGlobalContextForBlock ctxblk(&ctx);
  {
    dummy_renderer renderer;
    dummy_system_interface system_interface(m_log);
    dummy_file_interface file_interface(_mapping);

    Rml::SetRenderInterface(&renderer);
    Rml::SetSystemInterface(&system_interface);
    Rml::SetFileInterface(&file_interface);

    Rml::Initialise();
    base_document_instancer base_instancer(m_allocator, &includes, &out_string);
    Rml::Factory::RegisterElementInstancer("body", &base_instancer);

    dummy_tiled_image_instancer image_instancer(m_allocator, &includes);
    Rml::Factory::RegisterDecoratorInstancer("image", &image_instancer);

    Rml::Context* documents =
        Rml::CreateContext("main", Rml::Vector2i(1024, 1024));
    containers::string str(m_allocator, _file_name);
    Rml::ElementDocument* document = documents->LoadDocument(str.c_str());

    documents->UnloadAllDocuments();
    Rml::Shutdown();
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
    *_out_string += "Actor ";
    *_out_string += *_out_ui_name;
    *_out_string += " : RenderableUI {\n";
    *_out_string += " .name = \"";
    *_out_string += _file_name;
    *_out_string += "\";\n";
    *_out_string += " .class_name = \"";
    *_out_string += *_out_ui_name;
    *_out_string += "\";\n";
    *_out_string += out_string;
    *_out_string += "\n}\n\n ";

    *_out_string += *_out_ui_name;
    *_out_string += " getNew" + *_out_ui_name + " () { " + *_out_ui_name +
                    " x = " + *_out_ui_name + "(); \n" + "return x; }\n";

    return document != nullptr;
  }
}

}  // namespace ui
}  // namespace engine
}  // namespace wn
