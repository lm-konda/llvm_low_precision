//===-- TypeIndex.cpp - CodeView type index ---------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/DebugInfo/CodeView/TypeIndex.h"

#include "llvm/DebugInfo/CodeView/TypeCollection.h"
#include "llvm/Support/ScopedPrinter.h"

using namespace llvm;
using namespace llvm::codeview;

namespace {
struct SimpleTypeEntry {
  StringRef Name;
  SimpleTypeKind Kind;
};

/// The names here all end in "*". If the simple type is a pointer type, we
/// return the whole name. Otherwise we lop off the last character in our
/// StringRef.
static const SimpleTypeEntry SimpleTypeNames[] = {
    {"void*", SimpleTypeKind::Void},
    {"<not translated>*", SimpleTypeKind::NotTranslated},
    {"HRESULT*", SimpleTypeKind::HResult},
    {"int4*", SimpleTypeKind::SInt4}, // LMSDK
    {"unsigned int4*", SimpleTypeKind::UInt4}, // LMSDK
    {"signed char*", SimpleTypeKind::SignedCharacter},
    {"unsigned char*", SimpleTypeKind::UnsignedCharacter},
    {"char*", SimpleTypeKind::NarrowCharacter},
    {"wchar_t*", SimpleTypeKind::WideCharacter},
    {"char16_t*", SimpleTypeKind::Character16},
    {"char32_t*", SimpleTypeKind::Character32},
    {"__int4*", SimpleTypeKind::SInt4}, // LMSDK
    {"unsigned __int4*", SimpleTypeKind::UInt4}, // LMSDK
    {"__int8*", SimpleTypeKind::SByte},
    {"unsigned __int8*", SimpleTypeKind::Byte},
    {"short*", SimpleTypeKind::Int16Short},
    {"unsigned short*", SimpleTypeKind::UInt16Short},
    {"__int16*", SimpleTypeKind::Int16},
    {"unsigned __int16*", SimpleTypeKind::UInt16},
    {"long*", SimpleTypeKind::Int32Long},
    {"unsigned long*", SimpleTypeKind::UInt32Long},
    {"int*", SimpleTypeKind::Int32},
    {"unsigned*", SimpleTypeKind::UInt32},
    {"__int64*", SimpleTypeKind::Int64Quad},
    {"unsigned __int64*", SimpleTypeKind::UInt64Quad},
    {"__int64*", SimpleTypeKind::Int64},
    {"unsigned __int64*", SimpleTypeKind::UInt64},
    {"__int128*", SimpleTypeKind::Int128},
    {"unsigned __int128*", SimpleTypeKind::UInt128},
    {"__fixed4*", SimpleTypeKind::Fixed4}, // LMSDK
    {"__fixed8*", SimpleTypeKind::Fixed8}, // LMSDK    
    {"__half*", SimpleTypeKind::Float16},
    {"float*", SimpleTypeKind::Float32},
    {"float*", SimpleTypeKind::Float32PartialPrecision},
    {"__float48*", SimpleTypeKind::Float48},
    {"double*", SimpleTypeKind::Float64},
    {"long double*", SimpleTypeKind::Float80},
    {"__float128*", SimpleTypeKind::Float128},
    {"_Complex float*", SimpleTypeKind::Complex32},
    {"_Complex double*", SimpleTypeKind::Complex64},
    {"_Complex long double*", SimpleTypeKind::Complex80},
    {"_Complex __float128*", SimpleTypeKind::Complex128},
    {"bool*", SimpleTypeKind::Boolean8},
    {"__bool16*", SimpleTypeKind::Boolean16},
    {"__bool32*", SimpleTypeKind::Boolean32},
    {"__bool64*", SimpleTypeKind::Boolean64},
};
} // namespace

StringRef TypeIndex::simpleTypeName(TypeIndex TI) {
  assert(TI.isNoneType() || TI.isSimple());

  if (TI.isNoneType())
    return "<no type>";

  // This is a simple type.
  for (const auto &SimpleTypeName : SimpleTypeNames) {
    if (SimpleTypeName.Kind == TI.getSimpleKind()) {
      if (TI.getSimpleMode() == SimpleTypeMode::Direct)
        return SimpleTypeName.Name.drop_back(1);
      // Otherwise, this is a pointer type. We gloss over the distinction
      // between near, far, 64, 32, etc, and just give a pointer type.
      return SimpleTypeName.Name;
    }
  }
  return "<unknown simple type>";
}

void llvm::codeview::printTypeIndex(ScopedPrinter &Printer, StringRef FieldName,
                                    TypeIndex TI, TypeCollection &Types) {
  StringRef TypeName;
  if (!TI.isNoneType()) {
    if (TI.isSimple())
      TypeName = TypeIndex::simpleTypeName(TI);
    else
      TypeName = Types.getTypeName(TI);
  }

  if (!TypeName.empty())
    Printer.printHex(FieldName, TypeName, TI.getIndex());
  else
    Printer.printHex(FieldName, TI.getIndex());
}
