// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: from_fc.proto

#ifndef PROTOBUF_from_5ffc_2eproto__INCLUDED
#define PROTOBUF_from_5ffc_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "data.pb.h"
// @@protoc_insertion_point(includes)
namespace com {
namespace microsoft {
namespace cellsar {
namespace protobuf {
namespace fc {
class FcMessage;
class FcMessageDefaultTypeInternal;
extern FcMessageDefaultTypeInternal _FcMessage_default_instance_;
class PlmnData;
class PlmnDataDefaultTypeInternal;
extern PlmnDataDefaultTypeInternal _PlmnData_default_instance_;
class SmsData;
class SmsDataDefaultTypeInternal;
extern SmsDataDefaultTypeInternal _SmsData_default_instance_;
}  // namespace fc
}  // namespace protobuf
}  // namespace cellsar
}  // namespace microsoft
}  // namespace com

namespace com {
namespace microsoft {
namespace cellsar {
namespace protobuf {
namespace fc {

namespace protobuf_from_5ffc_2eproto {
// Internal implementation detail -- do not call these.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[];
  static const ::google::protobuf::uint32 offsets[];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static void InitDefaultsImpl();
};
void AddDescriptors();
void InitDefaults();
}  // namespace protobuf_from_5ffc_2eproto

// ===================================================================

class PlmnData : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:com.microsoft.cellsar.protobuf.fc.PlmnData) */ {
 public:
  PlmnData();
  virtual ~PlmnData();

  PlmnData(const PlmnData& from);

  inline PlmnData& operator=(const PlmnData& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  PlmnData(PlmnData&& from) noexcept
    : PlmnData() {
    *this = ::std::move(from);
  }

  inline PlmnData& operator=(PlmnData&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const PlmnData& default_instance();

  static inline const PlmnData* internal_default_instance() {
    return reinterpret_cast<const PlmnData*>(
               &_PlmnData_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(PlmnData* other);
  friend void swap(PlmnData& a, PlmnData& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline PlmnData* New() const PROTOBUF_FINAL { return New(NULL); }

  PlmnData* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const PlmnData& from);
  void MergeFrom(const PlmnData& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(PlmnData* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string mcc = 1;
  void clear_mcc();
  static const int kMccFieldNumber = 1;
  const ::std::string& mcc() const;
  void set_mcc(const ::std::string& value);
  #if LANG_CXX11
  void set_mcc(::std::string&& value);
  #endif
  void set_mcc(const char* value);
  void set_mcc(const char* value, size_t size);
  ::std::string* mutable_mcc();
  ::std::string* release_mcc();
  void set_allocated_mcc(::std::string* mcc);

  // string mnc = 2;
  void clear_mnc();
  static const int kMncFieldNumber = 2;
  const ::std::string& mnc() const;
  void set_mnc(const ::std::string& value);
  #if LANG_CXX11
  void set_mnc(::std::string&& value);
  #endif
  void set_mnc(const char* value);
  void set_mnc(const char* value, size_t size);
  ::std::string* mutable_mnc();
  ::std::string* release_mnc();
  void set_allocated_mnc(::std::string* mnc);

  // @@protoc_insertion_point(class_scope:com.microsoft.cellsar.protobuf.fc.PlmnData)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr mcc_;
  ::google::protobuf::internal::ArenaStringPtr mnc_;
  mutable int _cached_size_;
  friend struct protobuf_from_5ffc_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class SmsData : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:com.microsoft.cellsar.protobuf.fc.SmsData) */ {
 public:
  SmsData();
  virtual ~SmsData();

  SmsData(const SmsData& from);

  inline SmsData& operator=(const SmsData& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  SmsData(SmsData&& from) noexcept
    : SmsData() {
    *this = ::std::move(from);
  }

  inline SmsData& operator=(SmsData&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const SmsData& default_instance();

  static inline const SmsData* internal_default_instance() {
    return reinterpret_cast<const SmsData*>(
               &_SmsData_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(SmsData* other);
  friend void swap(SmsData& a, SmsData& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline SmsData* New() const PROTOBUF_FINAL { return New(NULL); }

  SmsData* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const SmsData& from);
  void MergeFrom(const SmsData& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(SmsData* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string msg = 2;
  void clear_msg();
  static const int kMsgFieldNumber = 2;
  const ::std::string& msg() const;
  void set_msg(const ::std::string& value);
  #if LANG_CXX11
  void set_msg(::std::string&& value);
  #endif
  void set_msg(const char* value);
  void set_msg(const char* value, size_t size);
  ::std::string* mutable_msg();
  ::std::string* release_msg();
  void set_allocated_msg(::std::string* msg);

  // .com.microsoft.cellsar.protobuf.Subscriber subscriber = 1;
  bool has_subscriber() const;
  void clear_subscriber();
  static const int kSubscriberFieldNumber = 1;
  const ::com::microsoft::cellsar::protobuf::Subscriber& subscriber() const;
  ::com::microsoft::cellsar::protobuf::Subscriber* mutable_subscriber();
  ::com::microsoft::cellsar::protobuf::Subscriber* release_subscriber();
  void set_allocated_subscriber(::com::microsoft::cellsar::protobuf::Subscriber* subscriber);

  // uint32 tries = 3;
  void clear_tries();
  static const int kTriesFieldNumber = 3;
  ::google::protobuf::uint32 tries() const;
  void set_tries(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:com.microsoft.cellsar.protobuf.fc.SmsData)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr msg_;
  ::com::microsoft::cellsar::protobuf::Subscriber* subscriber_;
  ::google::protobuf::uint32 tries_;
  mutable int _cached_size_;
  friend struct protobuf_from_5ffc_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class FcMessage : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:com.microsoft.cellsar.protobuf.fc.FcMessage) */ {
 public:
  FcMessage();
  virtual ~FcMessage();

  FcMessage(const FcMessage& from);

  inline FcMessage& operator=(const FcMessage& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  FcMessage(FcMessage&& from) noexcept
    : FcMessage() {
    *this = ::std::move(from);
  }

  inline FcMessage& operator=(FcMessage&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const FcMessage& default_instance();

  static inline const FcMessage* internal_default_instance() {
    return reinterpret_cast<const FcMessage*>(
               &_FcMessage_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    2;

  void Swap(FcMessage* other);
  friend void swap(FcMessage& a, FcMessage& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline FcMessage* New() const PROTOBUF_FINAL { return New(NULL); }

  FcMessage* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const FcMessage& from);
  void MergeFrom(const FcMessage& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(FcMessage* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string type = 1;
  void clear_type();
  static const int kTypeFieldNumber = 1;
  const ::std::string& type() const;
  void set_type(const ::std::string& value);
  #if LANG_CXX11
  void set_type(::std::string&& value);
  #endif
  void set_type(const char* value);
  void set_type(const char* value, size_t size);
  ::std::string* mutable_type();
  ::std::string* release_type();
  void set_allocated_type(::std::string* type);

  // .com.microsoft.cellsar.protobuf.fc.PlmnData plmn_data = 3;
  bool has_plmn_data() const;
  void clear_plmn_data();
  static const int kPlmnDataFieldNumber = 3;
  const ::com::microsoft::cellsar::protobuf::fc::PlmnData& plmn_data() const;
  ::com::microsoft::cellsar::protobuf::fc::PlmnData* mutable_plmn_data();
  ::com::microsoft::cellsar::protobuf::fc::PlmnData* release_plmn_data();
  void set_allocated_plmn_data(::com::microsoft::cellsar::protobuf::fc::PlmnData* plmn_data);

  // .com.microsoft.cellsar.protobuf.fc.SmsData sms_data = 4;
  bool has_sms_data() const;
  void clear_sms_data();
  static const int kSmsDataFieldNumber = 4;
  const ::com::microsoft::cellsar::protobuf::fc::SmsData& sms_data() const;
  ::com::microsoft::cellsar::protobuf::fc::SmsData* mutable_sms_data();
  ::com::microsoft::cellsar::protobuf::fc::SmsData* release_sms_data();
  void set_allocated_sms_data(::com::microsoft::cellsar::protobuf::fc::SmsData* sms_data);

  // uint64 time = 2;
  void clear_time();
  static const int kTimeFieldNumber = 2;
  ::google::protobuf::uint64 time() const;
  void set_time(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:com.microsoft.cellsar.protobuf.fc.FcMessage)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr type_;
  ::com::microsoft::cellsar::protobuf::fc::PlmnData* plmn_data_;
  ::com::microsoft::cellsar::protobuf::fc::SmsData* sms_data_;
  ::google::protobuf::uint64 time_;
  mutable int _cached_size_;
  friend struct protobuf_from_5ffc_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// PlmnData

// string mcc = 1;
inline void PlmnData::clear_mcc() {
  mcc_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& PlmnData::mcc() const {
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.PlmnData.mcc)
  return mcc_.GetNoArena();
}
inline void PlmnData::set_mcc(const ::std::string& value) {
  
  mcc_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:com.microsoft.cellsar.protobuf.fc.PlmnData.mcc)
}
#if LANG_CXX11
inline void PlmnData::set_mcc(::std::string&& value) {
  
  mcc_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:com.microsoft.cellsar.protobuf.fc.PlmnData.mcc)
}
#endif
inline void PlmnData::set_mcc(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  mcc_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:com.microsoft.cellsar.protobuf.fc.PlmnData.mcc)
}
inline void PlmnData::set_mcc(const char* value, size_t size) {
  
  mcc_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:com.microsoft.cellsar.protobuf.fc.PlmnData.mcc)
}
inline ::std::string* PlmnData::mutable_mcc() {
  
  // @@protoc_insertion_point(field_mutable:com.microsoft.cellsar.protobuf.fc.PlmnData.mcc)
  return mcc_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* PlmnData::release_mcc() {
  // @@protoc_insertion_point(field_release:com.microsoft.cellsar.protobuf.fc.PlmnData.mcc)
  
  return mcc_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void PlmnData::set_allocated_mcc(::std::string* mcc) {
  if (mcc != NULL) {
    
  } else {
    
  }
  mcc_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), mcc);
  // @@protoc_insertion_point(field_set_allocated:com.microsoft.cellsar.protobuf.fc.PlmnData.mcc)
}

// string mnc = 2;
inline void PlmnData::clear_mnc() {
  mnc_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& PlmnData::mnc() const {
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.PlmnData.mnc)
  return mnc_.GetNoArena();
}
inline void PlmnData::set_mnc(const ::std::string& value) {
  
  mnc_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:com.microsoft.cellsar.protobuf.fc.PlmnData.mnc)
}
#if LANG_CXX11
inline void PlmnData::set_mnc(::std::string&& value) {
  
  mnc_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:com.microsoft.cellsar.protobuf.fc.PlmnData.mnc)
}
#endif
inline void PlmnData::set_mnc(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  mnc_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:com.microsoft.cellsar.protobuf.fc.PlmnData.mnc)
}
inline void PlmnData::set_mnc(const char* value, size_t size) {
  
  mnc_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:com.microsoft.cellsar.protobuf.fc.PlmnData.mnc)
}
inline ::std::string* PlmnData::mutable_mnc() {
  
  // @@protoc_insertion_point(field_mutable:com.microsoft.cellsar.protobuf.fc.PlmnData.mnc)
  return mnc_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* PlmnData::release_mnc() {
  // @@protoc_insertion_point(field_release:com.microsoft.cellsar.protobuf.fc.PlmnData.mnc)
  
  return mnc_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void PlmnData::set_allocated_mnc(::std::string* mnc) {
  if (mnc != NULL) {
    
  } else {
    
  }
  mnc_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), mnc);
  // @@protoc_insertion_point(field_set_allocated:com.microsoft.cellsar.protobuf.fc.PlmnData.mnc)
}

// -------------------------------------------------------------------

// SmsData

// .com.microsoft.cellsar.protobuf.Subscriber subscriber = 1;
inline bool SmsData::has_subscriber() const {
  return this != internal_default_instance() && subscriber_ != NULL;
}
inline void SmsData::clear_subscriber() {
  if (GetArenaNoVirtual() == NULL && subscriber_ != NULL) delete subscriber_;
  subscriber_ = NULL;
}
inline const ::com::microsoft::cellsar::protobuf::Subscriber& SmsData::subscriber() const {
  const ::com::microsoft::cellsar::protobuf::Subscriber* p = subscriber_;
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.SmsData.subscriber)
  return p != NULL ? *p : *reinterpret_cast<const ::com::microsoft::cellsar::protobuf::Subscriber*>(
      &::com::microsoft::cellsar::protobuf::_Subscriber_default_instance_);
}
inline ::com::microsoft::cellsar::protobuf::Subscriber* SmsData::mutable_subscriber() {
  
  if (subscriber_ == NULL) {
    subscriber_ = new ::com::microsoft::cellsar::protobuf::Subscriber;
  }
  // @@protoc_insertion_point(field_mutable:com.microsoft.cellsar.protobuf.fc.SmsData.subscriber)
  return subscriber_;
}
inline ::com::microsoft::cellsar::protobuf::Subscriber* SmsData::release_subscriber() {
  // @@protoc_insertion_point(field_release:com.microsoft.cellsar.protobuf.fc.SmsData.subscriber)
  
  ::com::microsoft::cellsar::protobuf::Subscriber* temp = subscriber_;
  subscriber_ = NULL;
  return temp;
}
inline void SmsData::set_allocated_subscriber(::com::microsoft::cellsar::protobuf::Subscriber* subscriber) {
  delete subscriber_;
  subscriber_ = subscriber;
  if (subscriber) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:com.microsoft.cellsar.protobuf.fc.SmsData.subscriber)
}

// string msg = 2;
inline void SmsData::clear_msg() {
  msg_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& SmsData::msg() const {
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.SmsData.msg)
  return msg_.GetNoArena();
}
inline void SmsData::set_msg(const ::std::string& value) {
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:com.microsoft.cellsar.protobuf.fc.SmsData.msg)
}
#if LANG_CXX11
inline void SmsData::set_msg(::std::string&& value) {
  
  msg_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:com.microsoft.cellsar.protobuf.fc.SmsData.msg)
}
#endif
inline void SmsData::set_msg(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:com.microsoft.cellsar.protobuf.fc.SmsData.msg)
}
inline void SmsData::set_msg(const char* value, size_t size) {
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:com.microsoft.cellsar.protobuf.fc.SmsData.msg)
}
inline ::std::string* SmsData::mutable_msg() {
  
  // @@protoc_insertion_point(field_mutable:com.microsoft.cellsar.protobuf.fc.SmsData.msg)
  return msg_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* SmsData::release_msg() {
  // @@protoc_insertion_point(field_release:com.microsoft.cellsar.protobuf.fc.SmsData.msg)
  
  return msg_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void SmsData::set_allocated_msg(::std::string* msg) {
  if (msg != NULL) {
    
  } else {
    
  }
  msg_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), msg);
  // @@protoc_insertion_point(field_set_allocated:com.microsoft.cellsar.protobuf.fc.SmsData.msg)
}

// uint32 tries = 3;
inline void SmsData::clear_tries() {
  tries_ = 0u;
}
inline ::google::protobuf::uint32 SmsData::tries() const {
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.SmsData.tries)
  return tries_;
}
inline void SmsData::set_tries(::google::protobuf::uint32 value) {
  
  tries_ = value;
  // @@protoc_insertion_point(field_set:com.microsoft.cellsar.protobuf.fc.SmsData.tries)
}

// -------------------------------------------------------------------

// FcMessage

// string type = 1;
inline void FcMessage::clear_type() {
  type_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& FcMessage::type() const {
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.FcMessage.type)
  return type_.GetNoArena();
}
inline void FcMessage::set_type(const ::std::string& value) {
  
  type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:com.microsoft.cellsar.protobuf.fc.FcMessage.type)
}
#if LANG_CXX11
inline void FcMessage::set_type(::std::string&& value) {
  
  type_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:com.microsoft.cellsar.protobuf.fc.FcMessage.type)
}
#endif
inline void FcMessage::set_type(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:com.microsoft.cellsar.protobuf.fc.FcMessage.type)
}
inline void FcMessage::set_type(const char* value, size_t size) {
  
  type_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:com.microsoft.cellsar.protobuf.fc.FcMessage.type)
}
inline ::std::string* FcMessage::mutable_type() {
  
  // @@protoc_insertion_point(field_mutable:com.microsoft.cellsar.protobuf.fc.FcMessage.type)
  return type_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* FcMessage::release_type() {
  // @@protoc_insertion_point(field_release:com.microsoft.cellsar.protobuf.fc.FcMessage.type)
  
  return type_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void FcMessage::set_allocated_type(::std::string* type) {
  if (type != NULL) {
    
  } else {
    
  }
  type_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), type);
  // @@protoc_insertion_point(field_set_allocated:com.microsoft.cellsar.protobuf.fc.FcMessage.type)
}

// uint64 time = 2;
inline void FcMessage::clear_time() {
  time_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 FcMessage::time() const {
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.FcMessage.time)
  return time_;
}
inline void FcMessage::set_time(::google::protobuf::uint64 value) {
  
  time_ = value;
  // @@protoc_insertion_point(field_set:com.microsoft.cellsar.protobuf.fc.FcMessage.time)
}

// .com.microsoft.cellsar.protobuf.fc.PlmnData plmn_data = 3;
inline bool FcMessage::has_plmn_data() const {
  return this != internal_default_instance() && plmn_data_ != NULL;
}
inline void FcMessage::clear_plmn_data() {
  if (GetArenaNoVirtual() == NULL && plmn_data_ != NULL) delete plmn_data_;
  plmn_data_ = NULL;
}
inline const ::com::microsoft::cellsar::protobuf::fc::PlmnData& FcMessage::plmn_data() const {
  const ::com::microsoft::cellsar::protobuf::fc::PlmnData* p = plmn_data_;
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.FcMessage.plmn_data)
  return p != NULL ? *p : *reinterpret_cast<const ::com::microsoft::cellsar::protobuf::fc::PlmnData*>(
      &::com::microsoft::cellsar::protobuf::fc::_PlmnData_default_instance_);
}
inline ::com::microsoft::cellsar::protobuf::fc::PlmnData* FcMessage::mutable_plmn_data() {
  
  if (plmn_data_ == NULL) {
    plmn_data_ = new ::com::microsoft::cellsar::protobuf::fc::PlmnData;
  }
  // @@protoc_insertion_point(field_mutable:com.microsoft.cellsar.protobuf.fc.FcMessage.plmn_data)
  return plmn_data_;
}
inline ::com::microsoft::cellsar::protobuf::fc::PlmnData* FcMessage::release_plmn_data() {
  // @@protoc_insertion_point(field_release:com.microsoft.cellsar.protobuf.fc.FcMessage.plmn_data)
  
  ::com::microsoft::cellsar::protobuf::fc::PlmnData* temp = plmn_data_;
  plmn_data_ = NULL;
  return temp;
}
inline void FcMessage::set_allocated_plmn_data(::com::microsoft::cellsar::protobuf::fc::PlmnData* plmn_data) {
  delete plmn_data_;
  plmn_data_ = plmn_data;
  if (plmn_data) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:com.microsoft.cellsar.protobuf.fc.FcMessage.plmn_data)
}

// .com.microsoft.cellsar.protobuf.fc.SmsData sms_data = 4;
inline bool FcMessage::has_sms_data() const {
  return this != internal_default_instance() && sms_data_ != NULL;
}
inline void FcMessage::clear_sms_data() {
  if (GetArenaNoVirtual() == NULL && sms_data_ != NULL) delete sms_data_;
  sms_data_ = NULL;
}
inline const ::com::microsoft::cellsar::protobuf::fc::SmsData& FcMessage::sms_data() const {
  const ::com::microsoft::cellsar::protobuf::fc::SmsData* p = sms_data_;
  // @@protoc_insertion_point(field_get:com.microsoft.cellsar.protobuf.fc.FcMessage.sms_data)
  return p != NULL ? *p : *reinterpret_cast<const ::com::microsoft::cellsar::protobuf::fc::SmsData*>(
      &::com::microsoft::cellsar::protobuf::fc::_SmsData_default_instance_);
}
inline ::com::microsoft::cellsar::protobuf::fc::SmsData* FcMessage::mutable_sms_data() {
  
  if (sms_data_ == NULL) {
    sms_data_ = new ::com::microsoft::cellsar::protobuf::fc::SmsData;
  }
  // @@protoc_insertion_point(field_mutable:com.microsoft.cellsar.protobuf.fc.FcMessage.sms_data)
  return sms_data_;
}
inline ::com::microsoft::cellsar::protobuf::fc::SmsData* FcMessage::release_sms_data() {
  // @@protoc_insertion_point(field_release:com.microsoft.cellsar.protobuf.fc.FcMessage.sms_data)
  
  ::com::microsoft::cellsar::protobuf::fc::SmsData* temp = sms_data_;
  sms_data_ = NULL;
  return temp;
}
inline void FcMessage::set_allocated_sms_data(::com::microsoft::cellsar::protobuf::fc::SmsData* sms_data) {
  delete sms_data_;
  sms_data_ = sms_data;
  if (sms_data) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_set_allocated:com.microsoft.cellsar.protobuf.fc.FcMessage.sms_data)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)


}  // namespace fc
}  // namespace protobuf
}  // namespace cellsar
}  // namespace microsoft
}  // namespace com

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_from_5ffc_2eproto__INCLUDED
