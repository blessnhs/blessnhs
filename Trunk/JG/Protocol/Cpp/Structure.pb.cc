// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Structure.proto

#include "Structure.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
class RoomUserInfoDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<RoomUserInfo> _instance;
} _RoomUserInfo_default_instance_;
class RoomInfo2DefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<RoomInfo2> _instance;
} _RoomInfo2_default_instance_;
static void InitDefaultsscc_info_RoomInfo2_Structure_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_RoomInfo2_default_instance_;
    new (ptr) ::RoomInfo2();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::RoomInfo2::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_RoomInfo2_Structure_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_RoomInfo2_Structure_2eproto}, {}};

static void InitDefaultsscc_info_RoomUserInfo_Structure_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::_RoomUserInfo_default_instance_;
    new (ptr) ::RoomUserInfo();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::RoomUserInfo::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_RoomUserInfo_Structure_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_RoomUserInfo_Structure_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_Structure_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_Structure_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_Structure_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_Structure_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::RoomUserInfo, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::RoomUserInfo, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::RoomUserInfo, var_name_),
  PROTOBUF_FIELD_OFFSET(::RoomUserInfo, var_index_),
  0,
  1,
  PROTOBUF_FIELD_OFFSET(::RoomInfo2, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::RoomInfo2, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::RoomInfo2, var_id_),
  PROTOBUF_FIELD_OFFSET(::RoomInfo2, var_name_),
  PROTOBUF_FIELD_OFFSET(::RoomInfo2, var_current_count_),
  PROTOBUF_FIELD_OFFSET(::RoomInfo2, var_max_count_),
  1,
  0,
  2,
  3,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 7, sizeof(::RoomUserInfo)},
  { 9, 18, sizeof(::RoomInfo2)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_RoomUserInfo_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::_RoomInfo2_default_instance_),
};

const char descriptor_table_protodef_Structure_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\017Structure.proto\032\nEnum.proto\"3\n\014RoomUse"
  "rInfo\022\020\n\010var_name\030\001 \001(\014\022\021\n\tvar_index\030\002 \001"
  "(\003\"_\n\tRoomInfo2\022\016\n\006var_id\030\001 \001(\005\022\020\n\010var_n"
  "ame\030\002 \001(\014\022\031\n\021var_current_count\030\003 \001(\005\022\025\n\r"
  "var_max_count\030\004 \001(\005B\002H\001"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_Structure_2eproto_deps[1] = {
  &::descriptor_table_Enum_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_Structure_2eproto_sccs[2] = {
  &scc_info_RoomInfo2_Structure_2eproto.base,
  &scc_info_RoomUserInfo_Structure_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_Structure_2eproto_once;
static bool descriptor_table_Structure_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Structure_2eproto = {
  &descriptor_table_Structure_2eproto_initialized, descriptor_table_protodef_Structure_2eproto, "Structure.proto", 183,
  &descriptor_table_Structure_2eproto_once, descriptor_table_Structure_2eproto_sccs, descriptor_table_Structure_2eproto_deps, 2, 1,
  schemas, file_default_instances, TableStruct_Structure_2eproto::offsets,
  file_level_metadata_Structure_2eproto, 2, file_level_enum_descriptors_Structure_2eproto, file_level_service_descriptors_Structure_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_Structure_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_Structure_2eproto), true);

// ===================================================================

void RoomUserInfo::InitAsDefaultInstance() {
}
class RoomUserInfo::_Internal {
 public:
  using HasBits = decltype(std::declval<RoomUserInfo>()._has_bits_);
  static void set_has_var_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_var_index(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
};

RoomUserInfo::RoomUserInfo()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:RoomUserInfo)
}
RoomUserInfo::RoomUserInfo(const RoomUserInfo& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  var_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_var_name()) {
    var_name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.var_name_);
  }
  var_index_ = from.var_index_;
  // @@protoc_insertion_point(copy_constructor:RoomUserInfo)
}

void RoomUserInfo::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_RoomUserInfo_Structure_2eproto.base);
  var_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  var_index_ = PROTOBUF_LONGLONG(0);
}

RoomUserInfo::~RoomUserInfo() {
  // @@protoc_insertion_point(destructor:RoomUserInfo)
  SharedDtor();
}

void RoomUserInfo::SharedDtor() {
  var_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void RoomUserInfo::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const RoomUserInfo& RoomUserInfo::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_RoomUserInfo_Structure_2eproto.base);
  return *internal_default_instance();
}


void RoomUserInfo::Clear() {
// @@protoc_insertion_point(message_clear_start:RoomUserInfo)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    var_name_.ClearNonDefaultToEmptyNoArena();
  }
  var_index_ = PROTOBUF_LONGLONG(0);
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

const char* RoomUserInfo::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // optional bytes var_name = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_var_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional int64 var_index = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          _Internal::set_has_var_index(&has_bits);
          var_index_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* RoomUserInfo::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:RoomUserInfo)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional bytes var_name = 1;
  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteBytesMaybeAliased(
        1, this->_internal_var_name(), target);
  }

  // optional int64 var_index = 2;
  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt64ToArray(2, this->_internal_var_index(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:RoomUserInfo)
  return target;
}

size_t RoomUserInfo::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:RoomUserInfo)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    // optional bytes var_name = 1;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_var_name());
    }

    // optional int64 var_index = 2;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int64Size(
          this->_internal_var_index());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void RoomUserInfo::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:RoomUserInfo)
  GOOGLE_DCHECK_NE(&from, this);
  const RoomUserInfo* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<RoomUserInfo>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:RoomUserInfo)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:RoomUserInfo)
    MergeFrom(*source);
  }
}

void RoomUserInfo::MergeFrom(const RoomUserInfo& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:RoomUserInfo)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x00000003u) {
    if (cached_has_bits & 0x00000001u) {
      _has_bits_[0] |= 0x00000001u;
      var_name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.var_name_);
    }
    if (cached_has_bits & 0x00000002u) {
      var_index_ = from.var_index_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void RoomUserInfo::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:RoomUserInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RoomUserInfo::CopyFrom(const RoomUserInfo& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:RoomUserInfo)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RoomUserInfo::IsInitialized() const {
  return true;
}

void RoomUserInfo::InternalSwap(RoomUserInfo* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  var_name_.Swap(&other->var_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(var_index_, other->var_index_);
}

::PROTOBUF_NAMESPACE_ID::Metadata RoomUserInfo::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void RoomInfo2::InitAsDefaultInstance() {
}
class RoomInfo2::_Internal {
 public:
  using HasBits = decltype(std::declval<RoomInfo2>()._has_bits_);
  static void set_has_var_id(HasBits* has_bits) {
    (*has_bits)[0] |= 2u;
  }
  static void set_has_var_name(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static void set_has_var_current_count(HasBits* has_bits) {
    (*has_bits)[0] |= 4u;
  }
  static void set_has_var_max_count(HasBits* has_bits) {
    (*has_bits)[0] |= 8u;
  }
};

RoomInfo2::RoomInfo2()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:RoomInfo2)
}
RoomInfo2::RoomInfo2(const RoomInfo2& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  var_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_var_name()) {
    var_name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.var_name_);
  }
  ::memcpy(&var_id_, &from.var_id_,
    static_cast<size_t>(reinterpret_cast<char*>(&var_max_count_) -
    reinterpret_cast<char*>(&var_id_)) + sizeof(var_max_count_));
  // @@protoc_insertion_point(copy_constructor:RoomInfo2)
}

void RoomInfo2::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_RoomInfo2_Structure_2eproto.base);
  var_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(&var_id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&var_max_count_) -
      reinterpret_cast<char*>(&var_id_)) + sizeof(var_max_count_));
}

RoomInfo2::~RoomInfo2() {
  // @@protoc_insertion_point(destructor:RoomInfo2)
  SharedDtor();
}

void RoomInfo2::SharedDtor() {
  var_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void RoomInfo2::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const RoomInfo2& RoomInfo2::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_RoomInfo2_Structure_2eproto.base);
  return *internal_default_instance();
}


void RoomInfo2::Clear() {
// @@protoc_insertion_point(message_clear_start:RoomInfo2)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    var_name_.ClearNonDefaultToEmptyNoArena();
  }
  if (cached_has_bits & 0x0000000eu) {
    ::memset(&var_id_, 0, static_cast<size_t>(
        reinterpret_cast<char*>(&var_max_count_) -
        reinterpret_cast<char*>(&var_id_)) + sizeof(var_max_count_));
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear();
}

const char* RoomInfo2::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // optional int32 var_id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_var_id(&has_bits);
          var_id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional bytes var_name = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_var_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional int32 var_current_count = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 24)) {
          _Internal::set_has_var_current_count(&has_bits);
          var_current_count_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // optional int32 var_max_count = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 32)) {
          _Internal::set_has_var_max_count(&has_bits);
          var_max_count_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* RoomInfo2::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:RoomInfo2)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  // optional int32 var_id = 1;
  if (cached_has_bits & 0x00000002u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_var_id(), target);
  }

  // optional bytes var_name = 2;
  if (cached_has_bits & 0x00000001u) {
    target = stream->WriteBytesMaybeAliased(
        2, this->_internal_var_name(), target);
  }

  // optional int32 var_current_count = 3;
  if (cached_has_bits & 0x00000004u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(3, this->_internal_var_current_count(), target);
  }

  // optional int32 var_max_count = 4;
  if (cached_has_bits & 0x00000008u) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(4, this->_internal_var_max_count(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:RoomInfo2)
  return target;
}

size_t RoomInfo2::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:RoomInfo2)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x0000000fu) {
    // optional bytes var_name = 2;
    if (cached_has_bits & 0x00000001u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
          this->_internal_var_name());
    }

    // optional int32 var_id = 1;
    if (cached_has_bits & 0x00000002u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_var_id());
    }

    // optional int32 var_current_count = 3;
    if (cached_has_bits & 0x00000004u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_var_current_count());
    }

    // optional int32 var_max_count = 4;
    if (cached_has_bits & 0x00000008u) {
      total_size += 1 +
        ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
          this->_internal_var_max_count());
    }

  }
  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void RoomInfo2::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:RoomInfo2)
  GOOGLE_DCHECK_NE(&from, this);
  const RoomInfo2* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<RoomInfo2>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:RoomInfo2)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:RoomInfo2)
    MergeFrom(*source);
  }
}

void RoomInfo2::MergeFrom(const RoomInfo2& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:RoomInfo2)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  cached_has_bits = from._has_bits_[0];
  if (cached_has_bits & 0x0000000fu) {
    if (cached_has_bits & 0x00000001u) {
      _has_bits_[0] |= 0x00000001u;
      var_name_.AssignWithDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from.var_name_);
    }
    if (cached_has_bits & 0x00000002u) {
      var_id_ = from.var_id_;
    }
    if (cached_has_bits & 0x00000004u) {
      var_current_count_ = from.var_current_count_;
    }
    if (cached_has_bits & 0x00000008u) {
      var_max_count_ = from.var_max_count_;
    }
    _has_bits_[0] |= cached_has_bits;
  }
}

void RoomInfo2::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:RoomInfo2)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void RoomInfo2::CopyFrom(const RoomInfo2& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:RoomInfo2)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool RoomInfo2::IsInitialized() const {
  return true;
}

void RoomInfo2::InternalSwap(RoomInfo2* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  var_name_.Swap(&other->var_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(var_id_, other->var_id_);
  swap(var_current_count_, other->var_current_count_);
  swap(var_max_count_, other->var_max_count_);
}

::PROTOBUF_NAMESPACE_ID::Metadata RoomInfo2::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::RoomUserInfo* Arena::CreateMaybeMessage< ::RoomUserInfo >(Arena* arena) {
  return Arena::CreateInternal< ::RoomUserInfo >(arena);
}
template<> PROTOBUF_NOINLINE ::RoomInfo2* Arena::CreateMaybeMessage< ::RoomInfo2 >(Arena* arena) {
  return Arena::CreateInternal< ::RoomInfo2 >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
