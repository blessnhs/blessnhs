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
  ID_PKT_BROADCAST_ROOM_MESSAGE_REQ = 10,
  ID_PKT_BROADCAST_ROOM_MESSAGE_RES = 11,
  ID_PKT_NEW_USER_IN_ROOM_NTY = 12,
  ID_PKT_ROOM_LIST_REQ = 13,
  ID_PKT_ROOM_LIST_RES = 14,
  ID_PKT_MATCH_REQ = 15,
  ID_PKT_MATCH_RES = 16,
  ID_PKT_GAME_START_NTY = 17,
  ID_PKT_GAME_RESULT_NTY = 18,
  ID_PKT_RANK_REQ = 19,
  ID_PKT_RANK_RES = 20,
  ID_PKT_QNS_REQ = 21,
  ID_PKT_QNS_RES = 22,
  ID_PKT_ROOM_PASS_THROUGH_REQ = 23,
  ID_PKT_ROOM_PASS_THROUGH_RES = 24,
  ID_PKT_CANCEL_MATCH_REQ = 25,
  ID_PKT_CANCEL_MATCH_RES = 26,
  ID_PKT_NOTICE_REQ = 27,
  ID_PKT_NOTICE_RES = 28,
  ID_PKT_CHECK_NICKNAME_REQ = 29,
  ID_PKT_CHECK_NICKNAME_RES = 30,
  PROTOCOL_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  PROTOCOL_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool PROTOCOL_IsValid(int value);
constexpr PROTOCOL PROTOCOL_MIN = ID_PKT_VERSION_REQ;
constexpr PROTOCOL PROTOCOL_MAX = ID_PKT_CHECK_NICKNAME_RES;
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
  ErrorCode_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  ErrorCode_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool ErrorCode_IsValid(int value);
constexpr ErrorCode ErrorCode_MIN = Success;
constexpr ErrorCode ErrorCode_MAX = LoginFailed;
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
enum eTeam : int {
  None = 0,
  WHITE = 1,
  BLACK = 2,
  AIM = 3,
  AWHITE = 4,
  ABLACK = 5,
  eTeam_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  eTeam_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool eTeam_IsValid(int value);
constexpr eTeam eTeam_MIN = None;
constexpr eTeam eTeam_MAX = ABLACK;
constexpr int eTeam_ARRAYSIZE = eTeam_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* eTeam_descriptor();
template<typename T>
inline const std::string& eTeam_Name(T enum_t_value) {
  static_assert(::std::is_same<T, eTeam>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function eTeam_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    eTeam_descriptor(), enum_t_value);
}
inline bool eTeam_Parse(
    const std::string& name, eTeam* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<eTeam>(
    eTeam_descriptor(), name, value);
}
enum State : int {
  Prepare = 0,
  Game = 1,
  End = 2,
  State_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::min(),
  State_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::PROTOBUF_NAMESPACE_ID::int32>::max()
};
bool State_IsValid(int value);
constexpr State State_MIN = Prepare;
constexpr State State_MAX = End;
constexpr int State_ARRAYSIZE = State_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* State_descriptor();
template<typename T>
inline const std::string& State_Name(T enum_t_value) {
  static_assert(::std::is_same<T, State>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function State_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    State_descriptor(), enum_t_value);
}
inline bool State_Parse(
    const std::string& name, State* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<State>(
    State_descriptor(), name, value);
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
template <> struct is_proto_enum< ::eTeam> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::eTeam>() {
  return ::eTeam_descriptor();
}
template <> struct is_proto_enum< ::State> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::State>() {
  return ::State_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Enum_2eproto
