# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: CLI.GS.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


import Enum_pb2 as Enum__pb2
import Structure_pb2 as Structure__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='CLI.GS.proto',
  package='',
  syntax='proto2',
  serialized_options=b'H\001',
  serialized_pb=b'\n\x0c\x43LI.GS.proto\x1a\nEnum.proto\x1a\x0fStructure.proto\"8\n\x0bVERSION_REQ\x12)\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x12ID_PKT_VERSION_REQ\"U\n\tLOGIN_REQ\x12\'\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x10ID_PKT_LOGIN_REQ\x12\x0e\n\x06var_id\x18\x02 \x02(\t\x12\x0f\n\x07var_pwd\x18\x03 \x02(\t\"R\n\x0f\x43REATE_ROOM_REQ\x12-\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x16ID_PKT_CREATE_ROOM_REQ\x12\x10\n\x08var_name\x18\x02 \x02(\x0c\"N\n\x0e\x45NTER_ROOM_REQ\x12,\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x15ID_PKT_ENTER_ROOM_REQ\x12\x0e\n\x06var_id\x18\x02 \x02(\x05\"N\n\x0eLEAVE_ROOM_REQ\x12,\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x15ID_PKT_LEAVE_ROOM_REQ\x12\x0e\n\x06var_id\x18\x02 \x02(\x05\"<\n\rROOM_LIST_REQ\x12+\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x14ID_PKT_ROOM_LIST_REQ\"z\n\x15ROOM_PASS_THROUGH_REQ\x12\x33\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x1cID_PKT_ROOM_PASS_THROUGH_REQ\x12\x13\n\x0bvar_message\x18\x02 \x02(\x0c\x12\x17\n\x0fvar_message_int\x18\x03 \x02(\x05\"6\n\nNOTICE_REQ\x12(\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x11ID_PKT_NOTICE_REQ\"[\n\x12\x42ITMAP_MESSAGE_REQ\x12\x30\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x19ID_PKT_BITMAP_MESSAGE_REQ\x12\x13\n\x0bvar_message\x18\x02 \x03(\x0c\"Y\n\x11\x41UDIO_MESSAGE_REQ\x12/\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x18ID_PKT_AUDIO_MESSAGE_REQ\x12\x13\n\x0bvar_message\x18\x02 \x03(\x0c\"B\n\x10PRAY_MESSAGE_REQ\x12.\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x17ID_PKT_PRAY_MESSAGE_REQ\"_\n\x14PRAY_MESSAGE_REG_REQ\x12\x32\n\x02id\x18\x01 \x01(\x0e\x32\t.PROTOCOL:\x1bID_PKT_PRAY_MESSAGE_REG_REQ\x12\x13\n\x0bvar_message\x18\x02 \x01(\x0c\x42\x02H\x01'
  ,
  dependencies=[Enum__pb2.DESCRIPTOR,Structure__pb2.DESCRIPTOR,])




_VERSION_REQ = _descriptor.Descriptor(
  name='VERSION_REQ',
  full_name='VERSION_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='VERSION_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=45,
  serialized_end=101,
)


_LOGIN_REQ = _descriptor.Descriptor(
  name='LOGIN_REQ',
  full_name='LOGIN_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='LOGIN_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=2,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_id', full_name='LOGIN_REQ.var_id', index=1,
      number=2, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_pwd', full_name='LOGIN_REQ.var_pwd', index=2,
      number=3, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=103,
  serialized_end=188,
)


_CREATE_ROOM_REQ = _descriptor.Descriptor(
  name='CREATE_ROOM_REQ',
  full_name='CREATE_ROOM_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='CREATE_ROOM_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=4,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_name', full_name='CREATE_ROOM_REQ.var_name', index=1,
      number=2, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=190,
  serialized_end=272,
)


_ENTER_ROOM_REQ = _descriptor.Descriptor(
  name='ENTER_ROOM_REQ',
  full_name='ENTER_ROOM_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='ENTER_ROOM_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=6,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_id', full_name='ENTER_ROOM_REQ.var_id', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=274,
  serialized_end=352,
)


_LEAVE_ROOM_REQ = _descriptor.Descriptor(
  name='LEAVE_ROOM_REQ',
  full_name='LEAVE_ROOM_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='LEAVE_ROOM_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=8,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_id', full_name='LEAVE_ROOM_REQ.var_id', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=354,
  serialized_end=432,
)


_ROOM_LIST_REQ = _descriptor.Descriptor(
  name='ROOM_LIST_REQ',
  full_name='ROOM_LIST_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='ROOM_LIST_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=13,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=434,
  serialized_end=494,
)


_ROOM_PASS_THROUGH_REQ = _descriptor.Descriptor(
  name='ROOM_PASS_THROUGH_REQ',
  full_name='ROOM_PASS_THROUGH_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='ROOM_PASS_THROUGH_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=17,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_message', full_name='ROOM_PASS_THROUGH_REQ.var_message', index=1,
      number=2, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_message_int', full_name='ROOM_PASS_THROUGH_REQ.var_message_int', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=496,
  serialized_end=618,
)


_NOTICE_REQ = _descriptor.Descriptor(
  name='NOTICE_REQ',
  full_name='NOTICE_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='NOTICE_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=19,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=620,
  serialized_end=674,
)


_BITMAP_MESSAGE_REQ = _descriptor.Descriptor(
  name='BITMAP_MESSAGE_REQ',
  full_name='BITMAP_MESSAGE_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='BITMAP_MESSAGE_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=10,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_message', full_name='BITMAP_MESSAGE_REQ.var_message', index=1,
      number=2, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=676,
  serialized_end=767,
)


_AUDIO_MESSAGE_REQ = _descriptor.Descriptor(
  name='AUDIO_MESSAGE_REQ',
  full_name='AUDIO_MESSAGE_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='AUDIO_MESSAGE_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=21,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_message', full_name='AUDIO_MESSAGE_REQ.var_message', index=1,
      number=2, type=12, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=769,
  serialized_end=858,
)


_PRAY_MESSAGE_REQ = _descriptor.Descriptor(
  name='PRAY_MESSAGE_REQ',
  full_name='PRAY_MESSAGE_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='PRAY_MESSAGE_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=23,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=860,
  serialized_end=926,
)


_PRAY_MESSAGE_REG_REQ = _descriptor.Descriptor(
  name='PRAY_MESSAGE_REG_REQ',
  full_name='PRAY_MESSAGE_REG_REQ',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='PRAY_MESSAGE_REG_REQ.id', index=0,
      number=1, type=14, cpp_type=8, label=1,
      has_default_value=True, default_value=25,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_message', full_name='PRAY_MESSAGE_REG_REQ.var_message', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=b"",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=928,
  serialized_end=1023,
)

_VERSION_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_LOGIN_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_CREATE_ROOM_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_ENTER_ROOM_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_LEAVE_ROOM_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_ROOM_LIST_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_ROOM_PASS_THROUGH_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_NOTICE_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_BITMAP_MESSAGE_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_AUDIO_MESSAGE_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_PRAY_MESSAGE_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
_PRAY_MESSAGE_REG_REQ.fields_by_name['id'].enum_type = Enum__pb2._PROTOCOL
DESCRIPTOR.message_types_by_name['VERSION_REQ'] = _VERSION_REQ
DESCRIPTOR.message_types_by_name['LOGIN_REQ'] = _LOGIN_REQ
DESCRIPTOR.message_types_by_name['CREATE_ROOM_REQ'] = _CREATE_ROOM_REQ
DESCRIPTOR.message_types_by_name['ENTER_ROOM_REQ'] = _ENTER_ROOM_REQ
DESCRIPTOR.message_types_by_name['LEAVE_ROOM_REQ'] = _LEAVE_ROOM_REQ
DESCRIPTOR.message_types_by_name['ROOM_LIST_REQ'] = _ROOM_LIST_REQ
DESCRIPTOR.message_types_by_name['ROOM_PASS_THROUGH_REQ'] = _ROOM_PASS_THROUGH_REQ
DESCRIPTOR.message_types_by_name['NOTICE_REQ'] = _NOTICE_REQ
DESCRIPTOR.message_types_by_name['BITMAP_MESSAGE_REQ'] = _BITMAP_MESSAGE_REQ
DESCRIPTOR.message_types_by_name['AUDIO_MESSAGE_REQ'] = _AUDIO_MESSAGE_REQ
DESCRIPTOR.message_types_by_name['PRAY_MESSAGE_REQ'] = _PRAY_MESSAGE_REQ
DESCRIPTOR.message_types_by_name['PRAY_MESSAGE_REG_REQ'] = _PRAY_MESSAGE_REG_REQ
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

VERSION_REQ = _reflection.GeneratedProtocolMessageType('VERSION_REQ', (_message.Message,), {
  'DESCRIPTOR' : _VERSION_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:VERSION_REQ)
  })
_sym_db.RegisterMessage(VERSION_REQ)

LOGIN_REQ = _reflection.GeneratedProtocolMessageType('LOGIN_REQ', (_message.Message,), {
  'DESCRIPTOR' : _LOGIN_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:LOGIN_REQ)
  })
_sym_db.RegisterMessage(LOGIN_REQ)

CREATE_ROOM_REQ = _reflection.GeneratedProtocolMessageType('CREATE_ROOM_REQ', (_message.Message,), {
  'DESCRIPTOR' : _CREATE_ROOM_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:CREATE_ROOM_REQ)
  })
_sym_db.RegisterMessage(CREATE_ROOM_REQ)

ENTER_ROOM_REQ = _reflection.GeneratedProtocolMessageType('ENTER_ROOM_REQ', (_message.Message,), {
  'DESCRIPTOR' : _ENTER_ROOM_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:ENTER_ROOM_REQ)
  })
_sym_db.RegisterMessage(ENTER_ROOM_REQ)

LEAVE_ROOM_REQ = _reflection.GeneratedProtocolMessageType('LEAVE_ROOM_REQ', (_message.Message,), {
  'DESCRIPTOR' : _LEAVE_ROOM_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:LEAVE_ROOM_REQ)
  })
_sym_db.RegisterMessage(LEAVE_ROOM_REQ)

ROOM_LIST_REQ = _reflection.GeneratedProtocolMessageType('ROOM_LIST_REQ', (_message.Message,), {
  'DESCRIPTOR' : _ROOM_LIST_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:ROOM_LIST_REQ)
  })
_sym_db.RegisterMessage(ROOM_LIST_REQ)

ROOM_PASS_THROUGH_REQ = _reflection.GeneratedProtocolMessageType('ROOM_PASS_THROUGH_REQ', (_message.Message,), {
  'DESCRIPTOR' : _ROOM_PASS_THROUGH_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:ROOM_PASS_THROUGH_REQ)
  })
_sym_db.RegisterMessage(ROOM_PASS_THROUGH_REQ)

NOTICE_REQ = _reflection.GeneratedProtocolMessageType('NOTICE_REQ', (_message.Message,), {
  'DESCRIPTOR' : _NOTICE_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:NOTICE_REQ)
  })
_sym_db.RegisterMessage(NOTICE_REQ)

BITMAP_MESSAGE_REQ = _reflection.GeneratedProtocolMessageType('BITMAP_MESSAGE_REQ', (_message.Message,), {
  'DESCRIPTOR' : _BITMAP_MESSAGE_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:BITMAP_MESSAGE_REQ)
  })
_sym_db.RegisterMessage(BITMAP_MESSAGE_REQ)

AUDIO_MESSAGE_REQ = _reflection.GeneratedProtocolMessageType('AUDIO_MESSAGE_REQ', (_message.Message,), {
  'DESCRIPTOR' : _AUDIO_MESSAGE_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:AUDIO_MESSAGE_REQ)
  })
_sym_db.RegisterMessage(AUDIO_MESSAGE_REQ)

PRAY_MESSAGE_REQ = _reflection.GeneratedProtocolMessageType('PRAY_MESSAGE_REQ', (_message.Message,), {
  'DESCRIPTOR' : _PRAY_MESSAGE_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:PRAY_MESSAGE_REQ)
  })
_sym_db.RegisterMessage(PRAY_MESSAGE_REQ)

PRAY_MESSAGE_REG_REQ = _reflection.GeneratedProtocolMessageType('PRAY_MESSAGE_REG_REQ', (_message.Message,), {
  'DESCRIPTOR' : _PRAY_MESSAGE_REG_REQ,
  '__module__' : 'CLI.GS_pb2'
  # @@protoc_insertion_point(class_scope:PRAY_MESSAGE_REG_REQ)
  })
_sym_db.RegisterMessage(PRAY_MESSAGE_REG_REQ)


DESCRIPTOR._options = None
# @@protoc_insertion_point(module_scope)
