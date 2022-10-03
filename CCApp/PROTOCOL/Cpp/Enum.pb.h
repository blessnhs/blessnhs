// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Enum.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto

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
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Enum_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_Enum_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Enum_2eproto;
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE

enum PROTOCOL : int {
  ID_PKT_VERSION_REQ = 0,
  ID_PKT_VERSION_RES = 1,
  ID_PKT_LOGIN_REQ = 2,
  ID_PKT_LOGIN_RES = 3,
  ID_PKT_CREATE_ROOM_REQ = 4,
  ID_PKT_CREATE_ROOM_RES = 5,
  ID_PKT_ENTER_ROOM_REQ = 6,
  ID_PKT_ENTER_ROOM_RES = 7,
  ID_PKT_LEAVE_ROOM_REQ = 8,
  ID_PKT_LEAVE_ROOM_RES = 9,
  ID_PKT_BITMAP_MESSAGE_REQ = 10,
  ID_PKT_BITMAP_MESSAGE_RES = 11,
  ID_PKT_NEW_USER_IN_ROOM_NTY = 12,
  ID_PKT_ROOM_LIST_REQ = 13,
  ID_PKT_ROOM_LIST_RES = 14,
  ID_PKT_GAME_START_NTY = 15,
  ID_PKT_GAME_RESULT_NTY = 16,
  ID_PKT_ROOM_PASS_THROUGH_REQ = 17,
  ID_PKT_ROOM_PASS_THROUGH_RES = 18,
  ID_PKT_NOTICE_REQ = 19,
  ID_PKT_NOTICE_RES = 20,
  ID_PKT_AUDIO_MESSAGE_REQ = 21,
  ID_PKT_AUDIO_MESSAGE_RES = 22,
  ID_PKT_PRAY_MESSAGE_REQ = 23,
  ID_PKT_PRAY_MESSAGE_RES = 24,
  ID_PKT_PRAY_MESSAGE_REG_REQ = 25,
  ID_PKT_PRAY_MESSAGE_REG_RES = 26,
  ID_PKT_QNA_REQ = 27,
  ID_PKT_QNA_RES = 28,
  ID_PKT_MAIL_LIST_REQ = 29,
  ID_PKT_MAIL_LIST_RES = 30,
  ID_PKT_MAIL_SEND_REQ = 31,
  ID_PKT_MAIL_SEND_RES = 32,
  ID_PKT_MAIL_DEL_REQ = 33,
  ID_PKT_MAIL_DEL_RES = 34,
  ID_PKT_MAIL_READ_REQ = 35,
  ID_PKT_MAIL_READ_RES = 36,
  ID_PKT_REG_REQ = 37,
  ID_PKT_REG_RES = 38,
  ID_PKT_CLIENT_LOGOUT_REQ = 39,
  ID_PKT_CLIENT_LOGOUT_RES = 40,
  ID_PKT_CLIENT_KICK = 41,
  ID_PKT_CAMERA_CONTROL_REQ = 42,
  ID_PKT_CAMERA_CONTROL_RES = 43,
  ID_PKT_MPEG2TS_MESSAGE_REQ = 44,
  ID_PKT_MPEG2TS_MESSAGE_RES = 45,
  ID_PKT_CAMERA_WAKE_UP_REQ = 46,
  ID_PKT_CAMERA_WAKE_UP_RES = 47,
  ID_PKT_STOP_STREAM_REQ = 48,
  ID_PKT_STOP_STREAM_RES = 49,
  ID_PKT_REG_CAMERA_REQ = 50,
  ID_PKT_REG_CAMERA_RES = 51,
  ID_PKT_CAMERA_LIST_REQ = 52,
  ID_PKT_CAMERA_LIST_RES = 53,
  PROTOCOL_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  PROTOCOL_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool PROTOCOL_IsValid(int value);
constexpr PROTOCOL PROTOCOL_MIN = ID_PKT_VERSION_REQ;
constexpr PROTOCOL PROTOCOL_MAX = ID_PKT_CAMERA_LIST_RES;
constexpr int PROTOCOL_ARRAYSIZE = PROTOCOL_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* PROTOCOL_descriptor();
template<typename T>
inline const std::string& PROTOCOL_Name(T enum_t_value) {
  static_assert(::std::is_same<T, PROTOCOL>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function PROTOCOL_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    PROTOCOL_descriptor(), enum_t_value);
}
inline bool PROTOCOL_Parse(
    const std::string& name, PROTOCOL* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<PROTOCOL>(
    PROTOCOL_descriptor(), name, value);
}
enum ErrorCode : int {
  Success = 0,
  DataBaseError = 1,
  LoginDuplicate = 2,
  SystemError = 3,
  LoginFailed = 4,
  Duplicate_Enter_Room = 5,
  ErrorCode_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ErrorCode_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool ErrorCode_IsValid(int value);
constexpr ErrorCode ErrorCode_MIN = Success;
constexpr ErrorCode ErrorCode_MAX = Duplicate_Enter_Room;
constexpr int ErrorCode_ARRAYSIZE = ErrorCode_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ErrorCode_descriptor();
template<typename T>
inline const std::string& ErrorCode_Name(T enum_t_value) {
  static_assert(::std::is_same<T, ErrorCode>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function ErrorCode_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    ErrorCode_descriptor(), enum_t_value);
}
inline bool ErrorCode_Parse(
    const std::string& name, ErrorCode* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<ErrorCode>(
    ErrorCode_descriptor(), name, value);
}
enum CameraControlType : int {
  SwitchCamera = 0,
  QualityUp = 1,
  QualityDown = 2,
  Flash = 3,
  SaveFile = 4,
  CameraControlType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  CameraControlType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool CameraControlType_IsValid(int value);
constexpr CameraControlType CameraControlType_MIN = SwitchCamera;
constexpr CameraControlType CameraControlType_MAX = SaveFile;
constexpr int CameraControlType_ARRAYSIZE = CameraControlType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* CameraControlType_descriptor();
template<typename T>
inline const std::string& CameraControlType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, CameraControlType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function CameraControlType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    CameraControlType_descriptor(), enum_t_value);
}
inline bool CameraControlType_Parse(
    const std::string& name, CameraControlType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<CameraControlType>(
    CameraControlType_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)


PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::PROTOCOL> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::PROTOCOL>() {
  return ::PROTOCOL_descriptor();
}
template <> struct is_proto_enum< ::ErrorCode> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ErrorCode>() {
  return ::ErrorCode_descriptor();
}
template <> struct is_proto_enum< ::CameraControlType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::CameraControlType>() {
  return ::CameraControlType_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto
