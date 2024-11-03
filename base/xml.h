#pragma once

typedef struct {
    Str key, value;
} XmlParseAttribute;

typedef DARRAY(XmlParseAttribute, 64) XmlParseAttributeList;

internal void xml_parse(
    char* file_contents,
    size_t file_length,
    void* user_ctx,
    void (*on_element_open)(void* ctx, Str path, XmlParseAttributeList* attributes),
    void (*on_element_close)(void* ctx, Str path, Str content)
);