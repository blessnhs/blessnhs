# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: Structure.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='Structure.proto',
  package='',
  syntax='proto2',
  serialized_options=b'H\001',
  serialized_pb=b'\n\x0fStructure.proto\"H\n\x0cRoomUserInfo\x12\x10\n\x08var_name\x18\x01 \x01(\t\x12\x11\n\tvar_index\x18\x02 \x01(\x03\x12\x13\n\x0bpicture_uri\x18\x03 \x01(\t\"_\n\tRoomInfo2\x12\x0e\n\x06var_id\x18\x01 \x01(\x05\x12\x10\n\x08var_name\x18\x02 \x01(\t\x12\x19\n\x11var_current_count\x18\x03 \x01(\x05\x12\x15\n\rvar_max_count\x18\x04 \x01(\x05\"C\n\x08PrayInfo\x12\x10\n\x08var_name\x18\x01 \x01(\t\x12\x13\n\x0bvar_message\x18\x02 \x01(\t\x12\x10\n\x08var_time\x18\x03 \x01(\t\"C\n\nNotifyInfo\x12\x0e\n\x06var_id\x18\x01 \x01(\x03\x12\x13\n\x0bvar_content\x18\x02 \x01(\t\x12\x10\n\x08var_date\x18\x03 \x01(\tB\x02H\x01'
)




_ROOMUSERINFO = _descriptor.Descriptor(
  name='RoomUserInfo',
  full_name='RoomUserInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='var_name', full_name='RoomUserInfo.var_name', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_index', full_name='RoomUserInfo.var_index', index=1,
      number=2, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='picture_uri', full_name='RoomUserInfo.picture_uri', index=2,
      number=3, type=9, cpp_type=9, label=1,
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
  serialized_start=19,
  serialized_end=91,
)


_ROOMINFO2 = _descriptor.Descriptor(
  name='RoomInfo2',
  full_name='RoomInfo2',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='var_id', full_name='RoomInfo2.var_id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_name', full_name='RoomInfo2.var_name', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_current_count', full_name='RoomInfo2.var_current_count', index=2,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_max_count', full_name='RoomInfo2.var_max_count', index=3,
      number=4, type=5, cpp_type=1, label=1,
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
  serialized_start=93,
  serialized_end=188,
)


_PRAYINFO = _descriptor.Descriptor(
  name='PrayInfo',
  full_name='PrayInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='var_name', full_name='PrayInfo.var_name', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_message', full_name='PrayInfo.var_message', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_time', full_name='PrayInfo.var_time', index=2,
      number=3, type=9, cpp_type=9, label=1,
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
  serialized_start=190,
  serialized_end=257,
)


_NOTIFYINFO = _descriptor.Descriptor(
  name='NotifyInfo',
  full_name='NotifyInfo',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='var_id', full_name='NotifyInfo.var_id', index=0,
      number=1, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_content', full_name='NotifyInfo.var_content', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=b"".decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='var_date', full_name='NotifyInfo.var_date', index=2,
      number=3, type=9, cpp_type=9, label=1,
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
  serialized_start=259,
  serialized_end=326,
)

DESCRIPTOR.message_types_by_name['RoomUserInfo'] = _ROOMUSERINFO
DESCRIPTOR.message_types_by_name['RoomInfo2'] = _ROOMINFO2
DESCRIPTOR.message_types_by_name['PrayInfo'] = _PRAYINFO
DESCRIPTOR.message_types_by_name['NotifyInfo'] = _NOTIFYINFO
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

RoomUserInfo = _reflection.GeneratedProtocolMessageType('RoomUserInfo', (_message.Message,), {
  'DESCRIPTOR' : _ROOMUSERINFO,
  '__module__' : 'Structure_pb2'
  # @@protoc_insertion_point(class_scope:RoomUserInfo)
  })
_sym_db.RegisterMessage(RoomUserInfo)

RoomInfo2 = _reflection.GeneratedProtocolMessageType('RoomInfo2', (_message.Message,), {
  'DESCRIPTOR' : _ROOMINFO2,
  '__module__' : 'Structure_pb2'
  # @@protoc_insertion_point(class_scope:RoomInfo2)
  })
_sym_db.RegisterMessage(RoomInfo2)

PrayInfo = _reflection.GeneratedProtocolMessageType('PrayInfo', (_message.Message,), {
  'DESCRIPTOR' : _PRAYINFO,
  '__module__' : 'Structure_pb2'
  # @@protoc_insertion_point(class_scope:PrayInfo)
  })
_sym_db.RegisterMessage(PrayInfo)

NotifyInfo = _reflection.GeneratedProtocolMessageType('NotifyInfo', (_message.Message,), {
  'DESCRIPTOR' : _NOTIFYINFO,
  '__module__' : 'Structure_pb2'
  # @@protoc_insertion_point(class_scope:NotifyInfo)
  })
_sym_db.RegisterMessage(NotifyInfo)


DESCRIPTOR._options = None
# @@protoc_insertion_point(module_scope)
