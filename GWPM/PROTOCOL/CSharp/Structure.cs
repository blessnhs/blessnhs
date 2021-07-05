// <auto-generated>
//     Generated by the protocol buffer compiler.  DO NOT EDIT!
//     source: Structure.proto
// </auto-generated>
#pragma warning disable 1591, 0612, 3021
#region Designer generated code

using pb = global::Google.Protobuf;
using pbc = global::Google.Protobuf.Collections;
using pbr = global::Google.Protobuf.Reflection;
using scg = global::System.Collections.Generic;
/// <summary>Holder for reflection information generated from Structure.proto</summary>
public static partial class StructureReflection {

  #region Descriptor
  /// <summary>File descriptor for Structure.proto</summary>
  public static pbr::FileDescriptor Descriptor {
    get { return descriptor; }
  }
  private static pbr::FileDescriptor descriptor;

  static StructureReflection() {
    byte[] descriptorData = global::System.Convert.FromBase64String(
        string.Concat(
          "Cg9TdHJ1Y3R1cmUucHJvdG8iSAoMUm9vbVVzZXJJbmZvEhAKCHZhcl9uYW1l",
          "GAEgASgJEhEKCXZhcl9pbmRleBgCIAEoAxITCgtwaWN0dXJlX3VyaRgDIAEo",
          "CSJfCglSb29tSW5mbzISDgoGdmFyX2lkGAEgASgFEhAKCHZhcl9uYW1lGAIg",
          "ASgJEhkKEXZhcl9jdXJyZW50X2NvdW50GAMgASgFEhUKDXZhcl9tYXhfY291",
          "bnQYBCABKAUiQwoIUHJheUluZm8SEAoIdmFyX25hbWUYASABKAkSEwoLdmFy",
          "X21lc3NhZ2UYAiABKAkSEAoIdmFyX3RpbWUYAyABKAkiQwoKTm90aWZ5SW5m",
          "bxIOCgZ2YXJfaWQYASABKAMSEwoLdmFyX2NvbnRlbnQYAiABKAkSEAoIdmFy",
          "X2RhdGUYAyABKAlCAkgB"));
    descriptor = pbr::FileDescriptor.FromGeneratedCode(descriptorData,
        new pbr::FileDescriptor[] { },
        new pbr::GeneratedClrTypeInfo(null, null, new pbr::GeneratedClrTypeInfo[] {
          new pbr::GeneratedClrTypeInfo(typeof(global::RoomUserInfo), global::RoomUserInfo.Parser, new[]{ "VarName", "VarIndex", "PictureUri" }, null, null, null, null),
          new pbr::GeneratedClrTypeInfo(typeof(global::RoomInfo2), global::RoomInfo2.Parser, new[]{ "VarId", "VarName", "VarCurrentCount", "VarMaxCount" }, null, null, null, null),
          new pbr::GeneratedClrTypeInfo(typeof(global::PrayInfo), global::PrayInfo.Parser, new[]{ "VarName", "VarMessage", "VarTime" }, null, null, null, null),
          new pbr::GeneratedClrTypeInfo(typeof(global::NotifyInfo), global::NotifyInfo.Parser, new[]{ "VarId", "VarContent", "VarDate" }, null, null, null, null)
        }));
  }
  #endregion

}
#region Messages
public sealed partial class RoomUserInfo : pb::IMessage<RoomUserInfo> {
  private static readonly pb::MessageParser<RoomUserInfo> _parser = new pb::MessageParser<RoomUserInfo>(() => new RoomUserInfo());
  private pb::UnknownFieldSet _unknownFields;
  private int _hasBits0;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pb::MessageParser<RoomUserInfo> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::StructureReflection.Descriptor.MessageTypes[0]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public RoomUserInfo() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public RoomUserInfo(RoomUserInfo other) : this() {
    _hasBits0 = other._hasBits0;
    varName_ = other.varName_;
    varIndex_ = other.varIndex_;
    pictureUri_ = other.pictureUri_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public RoomUserInfo Clone() {
    return new RoomUserInfo(this);
  }

  /// <summary>Field number for the "var_name" field.</summary>
  public const int VarNameFieldNumber = 1;
  private readonly static string VarNameDefaultValue = "";

  private string varName_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string VarName {
    get { return varName_ ?? VarNameDefaultValue; }
    set {
      varName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }
  /// <summary>Gets whether the "var_name" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarName {
    get { return varName_ != null; }
  }
  /// <summary>Clears the value of the "var_name" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarName() {
    varName_ = null;
  }

  /// <summary>Field number for the "var_index" field.</summary>
  public const int VarIndexFieldNumber = 2;
  private readonly static long VarIndexDefaultValue = 0L;

  private long varIndex_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public long VarIndex {
    get { if ((_hasBits0 & 1) != 0) { return varIndex_; } else { return VarIndexDefaultValue; } }
    set {
      _hasBits0 |= 1;
      varIndex_ = value;
    }
  }
  /// <summary>Gets whether the "var_index" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarIndex {
    get { return (_hasBits0 & 1) != 0; }
  }
  /// <summary>Clears the value of the "var_index" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarIndex() {
    _hasBits0 &= ~1;
  }

  /// <summary>Field number for the "picture_uri" field.</summary>
  public const int PictureUriFieldNumber = 3;
  private readonly static string PictureUriDefaultValue = "";

  private string pictureUri_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string PictureUri {
    get { return pictureUri_ ?? PictureUriDefaultValue; }
    set {
      pictureUri_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }
  /// <summary>Gets whether the "picture_uri" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasPictureUri {
    get { return pictureUri_ != null; }
  }
  /// <summary>Clears the value of the "picture_uri" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearPictureUri() {
    pictureUri_ = null;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override bool Equals(object other) {
    return Equals(other as RoomUserInfo);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool Equals(RoomUserInfo other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (VarName != other.VarName) return false;
    if (VarIndex != other.VarIndex) return false;
    if (PictureUri != other.PictureUri) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override int GetHashCode() {
    int hash = 1;
    if (HasVarName) hash ^= VarName.GetHashCode();
    if (HasVarIndex) hash ^= VarIndex.GetHashCode();
    if (HasPictureUri) hash ^= PictureUri.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void WriteTo(pb::CodedOutputStream output) {
    if (HasVarName) {
      output.WriteRawTag(10);
      output.WriteString(VarName);
    }
    if (HasVarIndex) {
      output.WriteRawTag(16);
      output.WriteInt64(VarIndex);
    }
    if (HasPictureUri) {
      output.WriteRawTag(26);
      output.WriteString(PictureUri);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int CalculateSize() {
    int size = 0;
    if (HasVarName) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(VarName);
    }
    if (HasVarIndex) {
      size += 1 + pb::CodedOutputStream.ComputeInt64Size(VarIndex);
    }
    if (HasPictureUri) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(PictureUri);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(RoomUserInfo other) {
    if (other == null) {
      return;
    }
    if (other.HasVarName) {
      VarName = other.VarName;
    }
    if (other.HasVarIndex) {
      VarIndex = other.VarIndex;
    }
    if (other.HasPictureUri) {
      PictureUri = other.PictureUri;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(pb::CodedInputStream input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 10: {
          VarName = input.ReadString();
          break;
        }
        case 16: {
          VarIndex = input.ReadInt64();
          break;
        }
        case 26: {
          PictureUri = input.ReadString();
          break;
        }
      }
    }
  }

}

public sealed partial class RoomInfo2 : pb::IMessage<RoomInfo2> {
  private static readonly pb::MessageParser<RoomInfo2> _parser = new pb::MessageParser<RoomInfo2>(() => new RoomInfo2());
  private pb::UnknownFieldSet _unknownFields;
  private int _hasBits0;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pb::MessageParser<RoomInfo2> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::StructureReflection.Descriptor.MessageTypes[1]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public RoomInfo2() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public RoomInfo2(RoomInfo2 other) : this() {
    _hasBits0 = other._hasBits0;
    varId_ = other.varId_;
    varName_ = other.varName_;
    varCurrentCount_ = other.varCurrentCount_;
    varMaxCount_ = other.varMaxCount_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public RoomInfo2 Clone() {
    return new RoomInfo2(this);
  }

  /// <summary>Field number for the "var_id" field.</summary>
  public const int VarIdFieldNumber = 1;
  private readonly static int VarIdDefaultValue = 0;

  private int varId_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int VarId {
    get { if ((_hasBits0 & 1) != 0) { return varId_; } else { return VarIdDefaultValue; } }
    set {
      _hasBits0 |= 1;
      varId_ = value;
    }
  }
  /// <summary>Gets whether the "var_id" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarId {
    get { return (_hasBits0 & 1) != 0; }
  }
  /// <summary>Clears the value of the "var_id" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarId() {
    _hasBits0 &= ~1;
  }

  /// <summary>Field number for the "var_name" field.</summary>
  public const int VarNameFieldNumber = 2;
  private readonly static string VarNameDefaultValue = "";

  private string varName_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string VarName {
    get { return varName_ ?? VarNameDefaultValue; }
    set {
      varName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }
  /// <summary>Gets whether the "var_name" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarName {
    get { return varName_ != null; }
  }
  /// <summary>Clears the value of the "var_name" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarName() {
    varName_ = null;
  }

  /// <summary>Field number for the "var_current_count" field.</summary>
  public const int VarCurrentCountFieldNumber = 3;
  private readonly static int VarCurrentCountDefaultValue = 0;

  private int varCurrentCount_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int VarCurrentCount {
    get { if ((_hasBits0 & 2) != 0) { return varCurrentCount_; } else { return VarCurrentCountDefaultValue; } }
    set {
      _hasBits0 |= 2;
      varCurrentCount_ = value;
    }
  }
  /// <summary>Gets whether the "var_current_count" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarCurrentCount {
    get { return (_hasBits0 & 2) != 0; }
  }
  /// <summary>Clears the value of the "var_current_count" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarCurrentCount() {
    _hasBits0 &= ~2;
  }

  /// <summary>Field number for the "var_max_count" field.</summary>
  public const int VarMaxCountFieldNumber = 4;
  private readonly static int VarMaxCountDefaultValue = 0;

  private int varMaxCount_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int VarMaxCount {
    get { if ((_hasBits0 & 4) != 0) { return varMaxCount_; } else { return VarMaxCountDefaultValue; } }
    set {
      _hasBits0 |= 4;
      varMaxCount_ = value;
    }
  }
  /// <summary>Gets whether the "var_max_count" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarMaxCount {
    get { return (_hasBits0 & 4) != 0; }
  }
  /// <summary>Clears the value of the "var_max_count" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarMaxCount() {
    _hasBits0 &= ~4;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override bool Equals(object other) {
    return Equals(other as RoomInfo2);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool Equals(RoomInfo2 other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (VarId != other.VarId) return false;
    if (VarName != other.VarName) return false;
    if (VarCurrentCount != other.VarCurrentCount) return false;
    if (VarMaxCount != other.VarMaxCount) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override int GetHashCode() {
    int hash = 1;
    if (HasVarId) hash ^= VarId.GetHashCode();
    if (HasVarName) hash ^= VarName.GetHashCode();
    if (HasVarCurrentCount) hash ^= VarCurrentCount.GetHashCode();
    if (HasVarMaxCount) hash ^= VarMaxCount.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void WriteTo(pb::CodedOutputStream output) {
    if (HasVarId) {
      output.WriteRawTag(8);
      output.WriteInt32(VarId);
    }
    if (HasVarName) {
      output.WriteRawTag(18);
      output.WriteString(VarName);
    }
    if (HasVarCurrentCount) {
      output.WriteRawTag(24);
      output.WriteInt32(VarCurrentCount);
    }
    if (HasVarMaxCount) {
      output.WriteRawTag(32);
      output.WriteInt32(VarMaxCount);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int CalculateSize() {
    int size = 0;
    if (HasVarId) {
      size += 1 + pb::CodedOutputStream.ComputeInt32Size(VarId);
    }
    if (HasVarName) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(VarName);
    }
    if (HasVarCurrentCount) {
      size += 1 + pb::CodedOutputStream.ComputeInt32Size(VarCurrentCount);
    }
    if (HasVarMaxCount) {
      size += 1 + pb::CodedOutputStream.ComputeInt32Size(VarMaxCount);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(RoomInfo2 other) {
    if (other == null) {
      return;
    }
    if (other.HasVarId) {
      VarId = other.VarId;
    }
    if (other.HasVarName) {
      VarName = other.VarName;
    }
    if (other.HasVarCurrentCount) {
      VarCurrentCount = other.VarCurrentCount;
    }
    if (other.HasVarMaxCount) {
      VarMaxCount = other.VarMaxCount;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(pb::CodedInputStream input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 8: {
          VarId = input.ReadInt32();
          break;
        }
        case 18: {
          VarName = input.ReadString();
          break;
        }
        case 24: {
          VarCurrentCount = input.ReadInt32();
          break;
        }
        case 32: {
          VarMaxCount = input.ReadInt32();
          break;
        }
      }
    }
  }

}

public sealed partial class PrayInfo : pb::IMessage<PrayInfo> {
  private static readonly pb::MessageParser<PrayInfo> _parser = new pb::MessageParser<PrayInfo>(() => new PrayInfo());
  private pb::UnknownFieldSet _unknownFields;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pb::MessageParser<PrayInfo> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::StructureReflection.Descriptor.MessageTypes[2]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public PrayInfo() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public PrayInfo(PrayInfo other) : this() {
    varName_ = other.varName_;
    varMessage_ = other.varMessage_;
    varTime_ = other.varTime_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public PrayInfo Clone() {
    return new PrayInfo(this);
  }

  /// <summary>Field number for the "var_name" field.</summary>
  public const int VarNameFieldNumber = 1;
  private readonly static string VarNameDefaultValue = "";

  private string varName_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string VarName {
    get { return varName_ ?? VarNameDefaultValue; }
    set {
      varName_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }
  /// <summary>Gets whether the "var_name" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarName {
    get { return varName_ != null; }
  }
  /// <summary>Clears the value of the "var_name" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarName() {
    varName_ = null;
  }

  /// <summary>Field number for the "var_message" field.</summary>
  public const int VarMessageFieldNumber = 2;
  private readonly static string VarMessageDefaultValue = "";

  private string varMessage_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string VarMessage {
    get { return varMessage_ ?? VarMessageDefaultValue; }
    set {
      varMessage_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }
  /// <summary>Gets whether the "var_message" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarMessage {
    get { return varMessage_ != null; }
  }
  /// <summary>Clears the value of the "var_message" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarMessage() {
    varMessage_ = null;
  }

  /// <summary>Field number for the "var_time" field.</summary>
  public const int VarTimeFieldNumber = 3;
  private readonly static string VarTimeDefaultValue = "";

  private string varTime_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string VarTime {
    get { return varTime_ ?? VarTimeDefaultValue; }
    set {
      varTime_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }
  /// <summary>Gets whether the "var_time" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarTime {
    get { return varTime_ != null; }
  }
  /// <summary>Clears the value of the "var_time" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarTime() {
    varTime_ = null;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override bool Equals(object other) {
    return Equals(other as PrayInfo);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool Equals(PrayInfo other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (VarName != other.VarName) return false;
    if (VarMessage != other.VarMessage) return false;
    if (VarTime != other.VarTime) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override int GetHashCode() {
    int hash = 1;
    if (HasVarName) hash ^= VarName.GetHashCode();
    if (HasVarMessage) hash ^= VarMessage.GetHashCode();
    if (HasVarTime) hash ^= VarTime.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void WriteTo(pb::CodedOutputStream output) {
    if (HasVarName) {
      output.WriteRawTag(10);
      output.WriteString(VarName);
    }
    if (HasVarMessage) {
      output.WriteRawTag(18);
      output.WriteString(VarMessage);
    }
    if (HasVarTime) {
      output.WriteRawTag(26);
      output.WriteString(VarTime);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int CalculateSize() {
    int size = 0;
    if (HasVarName) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(VarName);
    }
    if (HasVarMessage) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(VarMessage);
    }
    if (HasVarTime) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(VarTime);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(PrayInfo other) {
    if (other == null) {
      return;
    }
    if (other.HasVarName) {
      VarName = other.VarName;
    }
    if (other.HasVarMessage) {
      VarMessage = other.VarMessage;
    }
    if (other.HasVarTime) {
      VarTime = other.VarTime;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(pb::CodedInputStream input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 10: {
          VarName = input.ReadString();
          break;
        }
        case 18: {
          VarMessage = input.ReadString();
          break;
        }
        case 26: {
          VarTime = input.ReadString();
          break;
        }
      }
    }
  }

}

public sealed partial class NotifyInfo : pb::IMessage<NotifyInfo> {
  private static readonly pb::MessageParser<NotifyInfo> _parser = new pb::MessageParser<NotifyInfo>(() => new NotifyInfo());
  private pb::UnknownFieldSet _unknownFields;
  private int _hasBits0;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pb::MessageParser<NotifyInfo> Parser { get { return _parser; } }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public static pbr::MessageDescriptor Descriptor {
    get { return global::StructureReflection.Descriptor.MessageTypes[3]; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  pbr::MessageDescriptor pb::IMessage.Descriptor {
    get { return Descriptor; }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public NotifyInfo() {
    OnConstruction();
  }

  partial void OnConstruction();

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public NotifyInfo(NotifyInfo other) : this() {
    _hasBits0 = other._hasBits0;
    varId_ = other.varId_;
    varContent_ = other.varContent_;
    varDate_ = other.varDate_;
    _unknownFields = pb::UnknownFieldSet.Clone(other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public NotifyInfo Clone() {
    return new NotifyInfo(this);
  }

  /// <summary>Field number for the "var_id" field.</summary>
  public const int VarIdFieldNumber = 1;
  private readonly static long VarIdDefaultValue = 0L;

  private long varId_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public long VarId {
    get { if ((_hasBits0 & 1) != 0) { return varId_; } else { return VarIdDefaultValue; } }
    set {
      _hasBits0 |= 1;
      varId_ = value;
    }
  }
  /// <summary>Gets whether the "var_id" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarId {
    get { return (_hasBits0 & 1) != 0; }
  }
  /// <summary>Clears the value of the "var_id" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarId() {
    _hasBits0 &= ~1;
  }

  /// <summary>Field number for the "var_content" field.</summary>
  public const int VarContentFieldNumber = 2;
  private readonly static string VarContentDefaultValue = "";

  private string varContent_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string VarContent {
    get { return varContent_ ?? VarContentDefaultValue; }
    set {
      varContent_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }
  /// <summary>Gets whether the "var_content" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarContent {
    get { return varContent_ != null; }
  }
  /// <summary>Clears the value of the "var_content" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarContent() {
    varContent_ = null;
  }

  /// <summary>Field number for the "var_date" field.</summary>
  public const int VarDateFieldNumber = 3;
  private readonly static string VarDateDefaultValue = "";

  private string varDate_;
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public string VarDate {
    get { return varDate_ ?? VarDateDefaultValue; }
    set {
      varDate_ = pb::ProtoPreconditions.CheckNotNull(value, "value");
    }
  }
  /// <summary>Gets whether the "var_date" field is set</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool HasVarDate {
    get { return varDate_ != null; }
  }
  /// <summary>Clears the value of the "var_date" field</summary>
  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void ClearVarDate() {
    varDate_ = null;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override bool Equals(object other) {
    return Equals(other as NotifyInfo);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public bool Equals(NotifyInfo other) {
    if (ReferenceEquals(other, null)) {
      return false;
    }
    if (ReferenceEquals(other, this)) {
      return true;
    }
    if (VarId != other.VarId) return false;
    if (VarContent != other.VarContent) return false;
    if (VarDate != other.VarDate) return false;
    return Equals(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override int GetHashCode() {
    int hash = 1;
    if (HasVarId) hash ^= VarId.GetHashCode();
    if (HasVarContent) hash ^= VarContent.GetHashCode();
    if (HasVarDate) hash ^= VarDate.GetHashCode();
    if (_unknownFields != null) {
      hash ^= _unknownFields.GetHashCode();
    }
    return hash;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public override string ToString() {
    return pb::JsonFormatter.ToDiagnosticString(this);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void WriteTo(pb::CodedOutputStream output) {
    if (HasVarId) {
      output.WriteRawTag(8);
      output.WriteInt64(VarId);
    }
    if (HasVarContent) {
      output.WriteRawTag(18);
      output.WriteString(VarContent);
    }
    if (HasVarDate) {
      output.WriteRawTag(26);
      output.WriteString(VarDate);
    }
    if (_unknownFields != null) {
      _unknownFields.WriteTo(output);
    }
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public int CalculateSize() {
    int size = 0;
    if (HasVarId) {
      size += 1 + pb::CodedOutputStream.ComputeInt64Size(VarId);
    }
    if (HasVarContent) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(VarContent);
    }
    if (HasVarDate) {
      size += 1 + pb::CodedOutputStream.ComputeStringSize(VarDate);
    }
    if (_unknownFields != null) {
      size += _unknownFields.CalculateSize();
    }
    return size;
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(NotifyInfo other) {
    if (other == null) {
      return;
    }
    if (other.HasVarId) {
      VarId = other.VarId;
    }
    if (other.HasVarContent) {
      VarContent = other.VarContent;
    }
    if (other.HasVarDate) {
      VarDate = other.VarDate;
    }
    _unknownFields = pb::UnknownFieldSet.MergeFrom(_unknownFields, other._unknownFields);
  }

  [global::System.Diagnostics.DebuggerNonUserCodeAttribute]
  public void MergeFrom(pb::CodedInputStream input) {
    uint tag;
    while ((tag = input.ReadTag()) != 0) {
      switch(tag) {
        default:
          _unknownFields = pb::UnknownFieldSet.MergeFieldFrom(_unknownFields, input);
          break;
        case 8: {
          VarId = input.ReadInt64();
          break;
        }
        case 18: {
          VarContent = input.ReadString();
          break;
        }
        case 26: {
          VarDate = input.ReadString();
          break;
        }
      }
    }
  }

}

#endregion


#endregion Designer generated code
