// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Enum.proto

#include "Enum.pb.h"

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
static constexpr ::PROTOBUF_NAMESPACE_ID::Metadata* file_level_metadata_Enum_2eproto = nullptr;
static const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* file_level_enum_descriptors_Enum_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_Enum_2eproto = nullptr;
const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_Enum_2eproto::offsets[1] = {};
static constexpr ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema* schemas = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::Message* const* file_default_instances = nullptr;

const char descriptor_table_protodef_Enum_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\nEnum.proto*\364\007\n\010PROTOCOL\022\026\n\022ID_PKT_VERS"
  "ION_REQ\020\000\022\026\n\022ID_PKT_VERSION_RES\020\001\022\024\n\020ID_"
  "PKT_LOGIN_REQ\020\002\022\024\n\020ID_PKT_LOGIN_RES\020\003\022\032\n"
  "\026ID_PKT_CREATE_ROOM_REQ\020\004\022\032\n\026ID_PKT_CREA"
  "TE_ROOM_RES\020\005\022\031\n\025ID_PKT_ENTER_ROOM_REQ\020\006"
  "\022\031\n\025ID_PKT_ENTER_ROOM_RES\020\007\022\031\n\025ID_PKT_LE"
  "AVE_ROOM_REQ\020\010\022\031\n\025ID_PKT_LEAVE_ROOM_RES\020"
  "\t\022\035\n\031ID_PKT_BITMAP_MESSAGE_REQ\020\n\022\035\n\031ID_P"
  "KT_BITMAP_MESSAGE_RES\020\013\022\037\n\033ID_PKT_NEW_US"
  "ER_IN_ROOM_NTY\020\014\022\030\n\024ID_PKT_ROOM_LIST_REQ"
  "\020\r\022\030\n\024ID_PKT_ROOM_LIST_RES\020\016\022\031\n\025ID_PKT_G"
  "AME_START_NTY\020\017\022\032\n\026ID_PKT_GAME_RESULT_NT"
  "Y\020\020\022 \n\034ID_PKT_ROOM_PASS_THROUGH_REQ\020\021\022 \n"
  "\034ID_PKT_ROOM_PASS_THROUGH_RES\020\022\022\025\n\021ID_PK"
  "T_NOTICE_REQ\020\023\022\025\n\021ID_PKT_NOTICE_RES\020\024\022\034\n"
  "\030ID_PKT_AUDIO_MESSAGE_REQ\020\025\022\034\n\030ID_PKT_AU"
  "DIO_MESSAGE_RES\020\026\022\033\n\027ID_PKT_PRAY_MESSAGE"
  "_REQ\020\027\022\033\n\027ID_PKT_PRAY_MESSAGE_RES\020\030\022\037\n\033I"
  "D_PKT_PRAY_MESSAGE_REG_REQ\020\031\022\037\n\033ID_PKT_P"
  "RAY_MESSAGE_REG_RES\020\032\022\022\n\016ID_PKT_QNA_REQ\020"
  "\033\022\022\n\016ID_PKT_QNA_RES\020\034\022\030\n\024ID_PKT_MAIL_LIS"
  "T_REQ\020\035\022\030\n\024ID_PKT_MAIL_LIST_RES\020\036\022\030\n\024ID_"
  "PKT_MAIL_SEND_REQ\020\037\022\030\n\024ID_PKT_MAIL_SEND_"
  "RES\020 \022\027\n\023ID_PKT_MAIL_DEL_REQ\020!\022\027\n\023ID_PKT"
  "_MAIL_DEL_RES\020\"\022\030\n\024ID_PKT_MAIL_READ_REQ\020"
  "#\022\030\n\024ID_PKT_MAIL_READ_RES\020$*{\n\tErrorCode"
  "\022\013\n\007Success\020\000\022\021\n\rDataBaseError\020\001\022\022\n\016Logi"
  "nDuplicate\020\002\022\017\n\013SystemError\020\003\022\017\n\013LoginFa"
  "iled\020\004\022\030\n\024Duplicate_Enter_Room\020\005B\002H\001b\006pr"
  "oto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_Enum_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_Enum_2eproto_sccs[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_Enum_2eproto_once;
static bool descriptor_table_Enum_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Enum_2eproto = {
  &descriptor_table_Enum_2eproto_initialized, descriptor_table_protodef_Enum_2eproto, "Enum.proto", 1164,
  &descriptor_table_Enum_2eproto_once, descriptor_table_Enum_2eproto_sccs, descriptor_table_Enum_2eproto_deps, 0, 0,
  schemas, file_default_instances, TableStruct_Enum_2eproto::offsets,
  file_level_metadata_Enum_2eproto, 0, file_level_enum_descriptors_Enum_2eproto, file_level_service_descriptors_Enum_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_Enum_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_Enum_2eproto), true);
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* PROTOCOL_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_Enum_2eproto);
  return file_level_enum_descriptors_Enum_2eproto[0];
}
bool PROTOCOL_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
      return true;
    default:
      return false;
  }
}

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ErrorCode_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_Enum_2eproto);
  return file_level_enum_descriptors_Enum_2eproto[1];
}
bool ErrorCode_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)
PROTOBUF_NAMESPACE_OPEN
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
