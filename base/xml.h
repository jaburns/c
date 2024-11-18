#pragma once

structdef(XmlParseAttribute) {
    Str key, value;
};
DefArrayTypes(XmlParseAttribute);

internal void xml_parse(
    Arena* arena,
    char* file_contents,
    size_t file_length,
    void* user_ctx,
    void (*on_element_open)(void* ctx, Str path, Slice_XmlParseAttribute attributes),
    void (*on_element_close)(void* ctx, Str path, Str content)
);