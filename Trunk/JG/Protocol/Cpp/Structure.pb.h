// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Structure.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_Structure_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_Structure_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3011000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3011002 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "Enum.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Structure_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_Structure_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Structure_2eproto;
class RoomInfo2;
class RoomInfo2DefaultTypeInternal;
extern RoomInfo2DefaultTypeInternal _RoomInfo2_default_instance_;
class RoomUserInfo;
class RoomUserInfoDefaultTypeInternal;
extern RoomUserInfoDefaultTypeInternal _RoomUserInfo_default_instance_;
PROTOBUF_NAMESPACE_OPEN
template<> ::RoomInfo2* Arena::CreateMaybeMessage<::RoomInfo2>(Arena*);
template<> ::RoomUserInfo* Arena::CreateMaybeMessage<::RoomUserInfo>(Arena*);
PROTOBUF_NAMESPACE_CLOSE

// ===================================================================

class RoomUserInfo :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:RoomUserInfo) */ {
 public:
  RoomUserInfo();
  virtual ~RoomUserInfo();

  RoomUserInfo(const RoomUserInfo& from);
  RoomUserInfo(RoomUserInfo&& from) noexcept
    : RoomUserInfo() {
    *this = ::std::move(from);
  }

  inline RoomUserInfo& operator=(const RoomUserInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline RoomUserInfo& operator=(RoomUserInfo&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const RoomUserInfo& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const RoomUserInfo* internal_default_instance() {
    return reinterpret_cast<const RoomUserInfo*>(
               &_RoomUserInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(RoomUserInfo& a, RoomUserInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(RoomUserInfo* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline RoomUserInfo* New() const final {
    return CreateMaybeMessage<RoomUserInfo>(nullptr);
  }

  RoomUserInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<RoomUserInfo>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const RoomUserInfo& from);
  void MergeFrom(const RoomUserInfo& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(RoomUserInfo* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "RoomUserInfo";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_Structure_2eproto);
    return ::descriptor_table_Structure_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kVarNameFieldNumber = 1,
    kPictureUriFieldNumber = 3,
    kVarIndexFieldNumber = 2,
  };
  // optional bytes var_name = 1;
  bool has_var_name() const;
  private:
  bool _internal_has_var_name() const;
  public:
  void clear_var_name();
  const std::string& var_name() const;
  void set_var_name(const std::string& value);
  void set_var_name(std::string&& value);
  void set_var_name(const char* value);
  void set_var_name(const void* value, size_t size);
  std::string* mutable_var_name();
  std::string* release_var_name();
  void set_allocated_var_name(std::string* var_name);
  private:
  const std::string& _internal_var_name() const;
  void _internal_set_var_name(const std::string& value);
  std::string* _internal_mutable_var_name();
  public:

  // optional bytes picture_uri = 3;
  bool has_picture_uri() const;
  private:
  bool _internal_has_picture_uri() const;
  public:
  void clear_picture_uri();
  const std::string& picture_uri() const;
  void set_picture_uri(const std::string& value);
  void set_picture_uri(std::string&& value);
  void set_picture_uri(const char* value);
  void set_picture_uri(const void* value, size_t size);
  std::string* mutable_picture_uri();
  std::string* release_picture_uri();
  void set_allocated_picture_uri(std::string* picture_uri);
  private:
  const std::string& _internal_picture_uri() const;
  void _internal_set_picture_uri(const std::string& value);
  std::string* _internal_mutable_picture_uri();
  public:

  // optional int64 var_index = 2;
  bool has_var_index() const;
  private:
  bool _internal_has_var_index() const;
  public:
  void clear_var_index();
  ::PROTOBUF_NAMESPACE_ID::int64 var_index() const;
  void set_var_index(::PROTOBUF_NAMESPACE_ID::int64 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int64 _internal_var_index() const;
  void _internal_set_var_index(::PROTOBUF_NAMESPACE_ID::int64 value);
  public:

  // @@protoc_insertion_point(class_scope:RoomUserInfo)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr var_name_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr picture_uri_;
  ::PROTOBUF_NAMESPACE_ID::int64 var_index_;
  friend struct ::TableStruct_Structure_2eproto;
};
// -------------------------------------------------------------------

class RoomInfo2 :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:RoomInfo2) */ {
 public:
  RoomInfo2();
  virtual ~RoomInfo2();

  RoomInfo2(const RoomInfo2& from);
  RoomInfo2(RoomInfo2&& from) noexcept
    : RoomInfo2() {
    *this = ::std::move(from);
  }

  inline RoomInfo2& operator=(const RoomInfo2& from) {
    CopyFrom(from);
    return *this;
  }
  inline RoomInfo2& operator=(RoomInfo2&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const RoomInfo2& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const RoomInfo2* internal_default_instance() {
    return reinterpret_cast<const RoomInfo2*>(
               &_RoomInfo2_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(RoomInfo2& a, RoomInfo2& b) {
    a.Swap(&b);
  }
  inline void Swap(RoomInfo2* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline RoomInfo2* New() const final {
    return CreateMaybeMessage<RoomInfo2>(nullptr);
  }

  RoomInfo2* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<RoomInfo2>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const RoomInfo2& from);
  void MergeFrom(const RoomInfo2& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(RoomInfo2* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "RoomInfo2";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_Structure_2eproto);
    return ::descriptor_table_Structure_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kVarNameFieldNumber = 2,
    kVarIdFieldNumber = 1,
    kVarCurrentCountFieldNumber = 3,
    kVarMaxCountFieldNumber = 4,
  };
  // optional bytes var_name = 2;
  bool has_var_name() const;
  private:
  bool _internal_has_var_name() const;
  public:
  void clear_var_name();
  const std::string& var_name() const;
  void set_var_name(const std::string& value);
  void set_var_name(std::string&& value);
  void set_var_name(const char* value);
  void set_var_name(const void* value, size_t size);
  std::string* mutable_var_name();
  std::string* release_var_name();
  void set_allocated_var_name(std::string* var_name);
  private:
  const std::string& _internal_var_name() const;
  void _internal_set_var_name(const std::string& value);
  std::string* _internal_mutable_var_name();
  public:

  // optional int32 var_id = 1;
  bool has_var_id() const;
  private:
  bool _internal_has_var_id() const;
  public:
  void clear_var_id();
  ::PROTOBUF_NAMESPACE_ID::int32 var_id() const;
  void set_var_id(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_var_id() const;
  void _internal_set_var_id(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // optional int32 var_current_count = 3;
  bool has_var_current_count() const;
  private:
  bool _internal_has_var_current_count() const;
  public:
  void clear_var_current_count();
  ::PROTOBUF_NAMESPACE_ID::int32 var_current_count() const;
  void set_var_current_count(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_var_current_count() const;
  void _internal_set_var_current_count(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // optional int32 var_max_count = 4;
  bool has_var_max_count() const;
  private:
  bool _internal_has_var_max_count() const;
  public:
  void clear_var_max_count();
  ::PROTOBUF_NAMESPACE_ID::int32 var_max_count() const;
  void set_var_max_count(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_var_max_count() const;
  void _internal_set_var_max_count(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:RoomInfo2)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr var_name_;
  ::PROTOBUF_NAMESPACE_ID::int32 var_id_;
  ::PROTOBUF_NAMESPACE_ID::int32 var_current_count_;
  ::PROTOBUF_NAMESPACE_ID::int32 var_max_count_;
  friend struct ::TableStruct_Structure_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// RoomUserInfo

// optional bytes var_name = 1;
inline bool RoomUserInfo::_internal_has_var_name() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool RoomUserInfo::has_var_name() const {
  return _internal_has_var_name();
}
inline void RoomUserInfo::clear_var_name() {
  var_name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& RoomUserInfo::var_name() const {
  // @@protoc_insertion_point(field_get:RoomUserInfo.var_name)
  return _internal_var_name();
}
inline void RoomUserInfo::set_var_name(const std::string& value) {
  _internal_set_var_name(value);
  // @@protoc_insertion_point(field_set:RoomUserInfo.var_name)
}
inline std::string* RoomUserInfo::mutable_var_name() {
  // @@protoc_insertion_point(field_mutable:RoomUserInfo.var_name)
  return _internal_mutable_var_name();
}
inline const std::string& RoomUserInfo::_internal_var_name() const {
  return var_name_.GetNoArena();
}
inline void RoomUserInfo::_internal_set_var_name(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  var_name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
}
inline void RoomUserInfo::set_var_name(std::string&& value) {
  _has_bits_[0] |= 0x00000001u;
  var_name_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:RoomUserInfo.var_name)
}
inline void RoomUserInfo::set_var_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _has_bits_[0] |= 0x00000001u;
  var_name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:RoomUserInfo.var_name)
}
inline void RoomUserInfo::set_var_name(const void* value, size_t size) {
  _has_bits_[0] |= 0x00000001u;
  var_name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:RoomUserInfo.var_name)
}
inline std::string* RoomUserInfo::_internal_mutable_var_name() {
  _has_bits_[0] |= 0x00000001u;
  return var_name_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* RoomUserInfo::release_var_name() {
  // @@protoc_insertion_point(field_release:RoomUserInfo.var_name)
  if (!_internal_has_var_name()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  return var_name_.ReleaseNonDefaultNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void RoomUserInfo::set_allocated_var_name(std::string* var_name) {
  if (var_name != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  var_name_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), var_name);
  // @@protoc_insertion_point(field_set_allocated:RoomUserInfo.var_name)
}

// optional int64 var_index = 2;
inline bool RoomUserInfo::_internal_has_var_index() const {
  bool value = (_has_bits_[0] & 0x00000004u) != 0;
  return value;
}
inline bool RoomUserInfo::has_var_index() const {
  return _internal_has_var_index();
}
inline void RoomUserInfo::clear_var_index() {
  var_index_ = PROTOBUF_LONGLONG(0);
  _has_bits_[0] &= ~0x00000004u;
}
inline ::PROTOBUF_NAMESPACE_ID::int64 RoomUserInfo::_internal_var_index() const {
  return var_index_;
}
inline ::PROTOBUF_NAMESPACE_ID::int64 RoomUserInfo::var_index() const {
  // @@protoc_insertion_point(field_get:RoomUserInfo.var_index)
  return _internal_var_index();
}
inline void RoomUserInfo::_internal_set_var_index(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _has_bits_[0] |= 0x00000004u;
  var_index_ = value;
}
inline void RoomUserInfo::set_var_index(::PROTOBUF_NAMESPACE_ID::int64 value) {
  _internal_set_var_index(value);
  // @@protoc_insertion_point(field_set:RoomUserInfo.var_index)
}

// optional bytes picture_uri = 3;
inline bool RoomUserInfo::_internal_has_picture_uri() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool RoomUserInfo::has_picture_uri() const {
  return _internal_has_picture_uri();
}
inline void RoomUserInfo::clear_picture_uri() {
  picture_uri_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  _has_bits_[0] &= ~0x00000002u;
}
inline const std::string& RoomUserInfo::picture_uri() const {
  // @@protoc_insertion_point(field_get:RoomUserInfo.picture_uri)
  return _internal_picture_uri();
}
inline void RoomUserInfo::set_picture_uri(const std::string& value) {
  _internal_set_picture_uri(value);
  // @@protoc_insertion_point(field_set:RoomUserInfo.picture_uri)
}
inline std::string* RoomUserInfo::mutable_picture_uri() {
  // @@protoc_insertion_point(field_mutable:RoomUserInfo.picture_uri)
  return _internal_mutable_picture_uri();
}
inline const std::string& RoomUserInfo::_internal_picture_uri() const {
  return picture_uri_.GetNoArena();
}
inline void RoomUserInfo::_internal_set_picture_uri(const std::string& value) {
  _has_bits_[0] |= 0x00000002u;
  picture_uri_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
}
inline void RoomUserInfo::set_picture_uri(std::string&& value) {
  _has_bits_[0] |= 0x00000002u;
  picture_uri_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:RoomUserInfo.picture_uri)
}
inline void RoomUserInfo::set_picture_uri(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _has_bits_[0] |= 0x00000002u;
  picture_uri_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:RoomUserInfo.picture_uri)
}
inline void RoomUserInfo::set_picture_uri(const void* value, size_t size) {
  _has_bits_[0] |= 0x00000002u;
  picture_uri_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:RoomUserInfo.picture_uri)
}
inline std::string* RoomUserInfo::_internal_mutable_picture_uri() {
  _has_bits_[0] |= 0x00000002u;
  return picture_uri_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* RoomUserInfo::release_picture_uri() {
  // @@protoc_insertion_point(field_release:RoomUserInfo.picture_uri)
  if (!_internal_has_picture_uri()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000002u;
  return picture_uri_.ReleaseNonDefaultNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void RoomUserInfo::set_allocated_picture_uri(std::string* picture_uri) {
  if (picture_uri != nullptr) {
    _has_bits_[0] |= 0x00000002u;
  } else {
    _has_bits_[0] &= ~0x00000002u;
  }
  picture_uri_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), picture_uri);
  // @@protoc_insertion_point(field_set_allocated:RoomUserInfo.picture_uri)
}

// -------------------------------------------------------------------

// RoomInfo2

// optional int32 var_id = 1;
inline bool RoomInfo2::_internal_has_var_id() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool RoomInfo2::has_var_id() const {
  return _internal_has_var_id();
}
inline void RoomInfo2::clear_var_id() {
  var_id_ = 0;
  _has_bits_[0] &= ~0x00000002u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RoomInfo2::_internal_var_id() const {
  return var_id_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RoomInfo2::var_id() const {
  // @@protoc_insertion_point(field_get:RoomInfo2.var_id)
  return _internal_var_id();
}
inline void RoomInfo2::_internal_set_var_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000002u;
  var_id_ = value;
}
inline void RoomInfo2::set_var_id(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_var_id(value);
  // @@protoc_insertion_point(field_set:RoomInfo2.var_id)
}

// optional bytes var_name = 2;
inline bool RoomInfo2::_internal_has_var_name() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool RoomInfo2::has_var_name() const {
  return _internal_has_var_name();
}
inline void RoomInfo2::clear_var_name() {
  var_name_.ClearToEmptyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& RoomInfo2::var_name() const {
  // @@protoc_insertion_point(field_get:RoomInfo2.var_name)
  return _internal_var_name();
}
inline void RoomInfo2::set_var_name(const std::string& value) {
  _internal_set_var_name(value);
  // @@protoc_insertion_point(field_set:RoomInfo2.var_name)
}
inline std::string* RoomInfo2::mutable_var_name() {
  // @@protoc_insertion_point(field_mutable:RoomInfo2.var_name)
  return _internal_mutable_var_name();
}
inline const std::string& RoomInfo2::_internal_var_name() const {
  return var_name_.GetNoArena();
}
inline void RoomInfo2::_internal_set_var_name(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  var_name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), value);
}
inline void RoomInfo2::set_var_name(std::string&& value) {
  _has_bits_[0] |= 0x00000001u;
  var_name_.SetNoArena(
    &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:RoomInfo2.var_name)
}
inline void RoomInfo2::set_var_name(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _has_bits_[0] |= 0x00000001u;
  var_name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:RoomInfo2.var_name)
}
inline void RoomInfo2::set_var_name(const void* value, size_t size) {
  _has_bits_[0] |= 0x00000001u;
  var_name_.SetNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:RoomInfo2.var_name)
}
inline std::string* RoomInfo2::_internal_mutable_var_name() {
  _has_bits_[0] |= 0x00000001u;
  return var_name_.MutableNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline std::string* RoomInfo2::release_var_name() {
  // @@protoc_insertion_point(field_release:RoomInfo2.var_name)
  if (!_internal_has_var_name()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  return var_name_.ReleaseNonDefaultNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}
inline void RoomInfo2::set_allocated_var_name(std::string* var_name) {
  if (var_name != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  var_name_.SetAllocatedNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), var_name);
  // @@protoc_insertion_point(field_set_allocated:RoomInfo2.var_name)
}

// optional int32 var_current_count = 3;
inline bool RoomInfo2::_internal_has_var_current_count() const {
  bool value = (_has_bits_[0] & 0x00000004u) != 0;
  return value;
}
inline bool RoomInfo2::has_var_current_count() const {
  return _internal_has_var_current_count();
}
inline void RoomInfo2::clear_var_current_count() {
  var_current_count_ = 0;
  _has_bits_[0] &= ~0x00000004u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RoomInfo2::_internal_var_current_count() const {
  return var_current_count_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RoomInfo2::var_current_count() const {
  // @@protoc_insertion_point(field_get:RoomInfo2.var_current_count)
  return _internal_var_current_count();
}
inline void RoomInfo2::_internal_set_var_current_count(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000004u;
  var_current_count_ = value;
}
inline void RoomInfo2::set_var_current_count(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_var_current_count(value);
  // @@protoc_insertion_point(field_set:RoomInfo2.var_current_count)
}

// optional int32 var_max_count = 4;
inline bool RoomInfo2::_internal_has_var_max_count() const {
  bool value = (_has_bits_[0] & 0x00000008u) != 0;
  return value;
}
inline bool RoomInfo2::has_var_max_count() const {
  return _internal_has_var_max_count();
}
inline void RoomInfo2::clear_var_max_count() {
  var_max_count_ = 0;
  _has_bits_[0] &= ~0x00000008u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RoomInfo2::_internal_var_max_count() const {
  return var_max_count_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RoomInfo2::var_max_count() const {
  // @@protoc_insertion_point(field_get:RoomInfo2.var_max_count)
  return _internal_var_max_count();
}
inline void RoomInfo2::_internal_set_var_max_count(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000008u;
  var_max_count_ = value;
}
inline void RoomInfo2::set_var_max_count(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_var_max_count(value);
  // @@protoc_insertion_point(field_set:RoomInfo2.var_max_count)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Structure_2eproto
