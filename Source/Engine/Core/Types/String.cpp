// Copyright (c) 2012-2020 Wojciech Figat. All rights reserved.

#include "String.h"
#include "StringView.h"
#include "Engine/Core/Collections/Array.h"

String String::Empty;

String::String(const StringAnsi& str)
{
    Set(str.Get(), str.Length());
}

String::String(const StringView& str)
{
    Set(str.Get(), str.Length());
}

String::String(const StringAnsiView& str)
{
    Set(str.Get(), str.Length());
}

String& String::operator+=(const StringView& str)
{
    Append(str.Get(), str.Length());
    return *this;
}

String& String::operator=(const StringView& s)
{
    Set(s.Get(), s.Length());
    return *this;
}

void String::Insert(int32 startIndex, const String& other)
{
    ASSERT(other._data != _data);
    const int32 myLength = Length();
    const int32 otherLength = other.Length();
    ASSERT(startIndex >= 0 && startIndex <= myLength);

    if (otherLength == 0)
        return;

    if (myLength == 0)
    {
        operator=(other);
        return;
    }

    const auto oldData = _data;
    const auto oldLength = _length;

    _length = oldLength + otherLength;
    _data = (Char*)Platform::Allocate((_length + 1) * sizeof(Char), 16);

    Platform::MemoryCopy(_data, oldData, startIndex * sizeof(Char));
    Platform::MemoryCopy(_data + startIndex, other.Get(), otherLength * sizeof(Char));
    Platform::MemoryCopy(_data + startIndex + otherLength, oldData + startIndex, (oldLength - startIndex) * sizeof(Char));
    _data[_length] = 0;

    Platform::Free(oldData);
}

void String::Remove(int32 startIndex, int32 length)
{
    const auto oldData = _data;
    const auto oldLength = _length;
    ASSERT(startIndex >= 0 && startIndex + length <= oldLength);

    if (startIndex == 0 && oldLength == length)
    {
        Clear();
        return;
    }

    _length = oldLength - length;
    _data = (Char*)Platform::Allocate((_length + 1) * sizeof(Char), 16);

    Platform::MemoryCopy(_data, oldData, startIndex * sizeof(Char));
    Platform::MemoryCopy(_data + startIndex, oldData + startIndex + length, (_length - startIndex) * sizeof(Char));
    _data[_length] = 0;

    Platform::Free(oldData);
}

void String::Split(Char c, Array<String>& results) const
{
    results.Clear();
    int32 start = 0;
    int32 length = Length();

    for (int32 i = 0; i < length; i++)
    {
        if (_data[i] == c)
        {
            int32 count = i - start;
            if (count > 0)
            {
                results.Add(Substring(start, count));
            }
            start = i + 1;
        }
    }

    const int32 count = length - start;
    if (count > 0)
    {
        results.Add(Substring(start, count));
    }
}

String String::GetFirstLine() const
{
    const int32 lineTerminatorIndex = Find(TEXT('\n'));
    return lineTerminatorIndex == -1 ? *this : Left(lineTerminatorIndex);
}

bool String::IsANSI() const
{
    bool result = true;
    for (int32 i = 0; i < _length; i++)
    {
        if (_data[i] > 255)
        {
            result = false;
            break;
        }
    }
    return result;
}

bool String::StartsWith(const StringView& prefix, StringSearchCase searchCase) const
{
    if (prefix.IsEmpty() || prefix.Length() > Length())
        return false;
    if (searchCase == StringSearchCase::IgnoreCase)
        return !StringUtils::CompareIgnoreCase(this->GetText(), *prefix, prefix.Length());
    return !StringUtils::Compare(this->GetText(), *prefix, prefix.Length());
}

bool String::EndsWith(const StringView& suffix, StringSearchCase searchCase) const
{
    if (suffix.IsEmpty() || suffix.Length() > Length())
        return false;
    if (searchCase == StringSearchCase::IgnoreCase)
        return !StringUtils::CompareIgnoreCase(&(*this)[Length() - suffix.Length()], *suffix);
    return !StringUtils::Compare(&(*this)[Length() - suffix.Length()], *suffix);
}

void String::TrimToNullTerminator()
{
    const int32 length = _length;
    const int32 newLength = StringUtils::Length(_data);
    if (length != 0 && length != newLength)
    {
        Resize(newLength);
    }
}

String String::TrimTrailing() const
{
    if (IsEmpty())
        return *this;

    int32 start = 0;
    int32 end = Length() - 1;

    while (start <= end)
    {
        if (!StringUtils::IsWhitespace((*this)[start]))
            break;
        start++;
    }
    while (end >= 0)
    {
        if (!StringUtils::IsWhitespace((*this)[end]))
            break;
        end--;
    }

    ASSERT_LOW_LAYER(end >= start);
    return Substring(start, end - start + 1);
}

String& String::operator/=(const Char* str)
{
    const int32 length = _length;
    if (length > 0 && _data[length - 1] != TEXT('/') && _data[length - 1] != TEXT('\\')
        && (str == nullptr || (str[0] != TEXT('/') && str[0] != TEXT('\\'))))
    {
        *this += TEXT('/');
    }
    return *this += str;
}

String& String::operator/=(const char* str)
{
    const int32 length = _length;
    if (length > 0 && _data[length - 1] != TEXT('/') && _data[length - 1] != TEXT('\\')
        && (str == nullptr || (str[0] != TEXT('/') && str[0] != TEXT('\\'))))
    {
        *this += TEXT('/');
    }
    return *this += str;
}

String& String::operator/=(const Char c)
{
    const int32 length = _length;
    if (length > 0 && _data[length - 1] != TEXT('/') && _data[length - 1] != TEXT('\\'))
    {
        *this += TEXT('/');
    }
    return *this += c;
}

String& String::operator/=(const StringView& str)
{
    const int32 length = _length;
    if (length > 1 && _data[length - 1] != TEXT('/') && _data[length - 1] != TEXT('\\')
        && (str == nullptr || (str[0] != TEXT('/') && str[0] != TEXT('\\'))))
    {
        *this += TEXT('/');
    }
    return *this += str;
}

StringAnsi String::ToStringAnsi() const
{
    return StringAnsi(*this);
}

StringAnsi StringAnsi::Empty;

StringAnsi::StringAnsi(const StringView& str)
{
    Set(str.Get(), str.Length());
}

StringAnsi::StringAnsi(const StringAnsiView& str)
{
    Set(str.Get(), str.Length());
}

StringAnsi& StringAnsi::operator+=(const StringAnsiView& str)
{
    Append(str.Get(), str.Length());
    return *this;
}

StringAnsi& StringAnsi::operator=(const StringAnsiView& s)
{
    Set(s.Get(), s.Length());
    return *this;
}

bool StringAnsi::StartsWith(const StringAnsiView& prefix, StringSearchCase searchCase) const
{
    if (prefix.IsEmpty() || prefix.Length() > Length())
        return false;
    if (searchCase == StringSearchCase::IgnoreCase)
        return !StringUtils::CompareIgnoreCase(this->GetText(), *prefix, prefix.Length());
    return !StringUtils::Compare(this->GetText(), *prefix, prefix.Length());
}

bool StringAnsi::EndsWith(const StringAnsiView& suffix, StringSearchCase searchCase) const
{
    if (suffix.IsEmpty() || suffix.Length() > Length())
        return false;
    if (searchCase == StringSearchCase::IgnoreCase)
        return !StringUtils::CompareIgnoreCase(&(*this)[Length() - suffix.Length()], *suffix);
    return !StringUtils::Compare(&(*this)[Length() - suffix.Length()], *suffix);
}

void StringAnsi::Insert(int32 startIndex, const StringAnsi& other)
{
    ASSERT(other._data != _data);
    const int32 myLength = Length();
    const int32 otherLength = other.Length();
    ASSERT(startIndex >= 0 && startIndex < _length);

    if (otherLength == 0)
        return;

    if (myLength == 0)
    {
        operator=(other);
        return;
    }

    const auto oldData = _data;
    const auto oldLength = _length;

    _length = oldLength + otherLength;
    _data = (char*)Platform::Allocate((_length + 1) * sizeof(char), 16);

    Platform::MemoryCopy(_data, oldData, startIndex * sizeof(char));
    Platform::MemoryCopy(_data + startIndex, other.Get(), otherLength * sizeof(char));
    Platform::MemoryCopy(_data + startIndex + otherLength, oldData + startIndex, (oldLength - startIndex) * sizeof(char));
    _data[_length] = 0;

    Platform::Free(oldData);
}

void StringAnsi::Remove(int32 startIndex, int32 length)
{
    const auto oldData = _data;
    const auto oldLength = _length;
    ASSERT(startIndex >= 0 && startIndex + length <= oldLength);

    if (startIndex == 0 && oldLength == length)
    {
        Clear();
        return;
    }

    _length = oldLength - length;
    _data = (char*)Platform::Allocate((_length + 1) * sizeof(char), 16);

    Platform::MemoryCopy(_data, oldData, startIndex * sizeof(char));
    Platform::MemoryCopy(_data + startIndex, oldData + startIndex + length, length * sizeof(char));
    _data[_length] = 0;

    Platform::Free(oldData);
}

void StringAnsi::Split(char c, Array<StringAnsi>& results) const
{
    results.Clear();
    int32 start = 0;
    int32 length = Length();

    for (int32 i = 0; i < length; i++)
    {
        if (_data[i] == c)
        {
            int32 count = i - start;
            if (count > 0)
            {
                results.Add(Substring(start, count));
            }
            start = i + 1;
        }
    }

    const int32 count = length - start;
    if (count > 0)
    {
        results.Add(Substring(start, count));
    }
}
