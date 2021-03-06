-- -*- coding: utf-8 -*-
--
-- Copyright 2010 Jeffrey Friedl
-- http://regex.info/blog/
--
-- for XLLoadModule so SINAWEIBO must be a GLOBAL variable
OBJDEF = {
   VERSION = 20100810.2 -- version hitory at end of file
}

--
-- Simple JSON encoding and decoding in pure Lua.
-- http://www.json.org/
--
--
--   JSON = (loadfile "JSON:lua")() -- one-time load of the routines
--
--   local lua_value = JSON:decode(raw_json_text)
--
--   local raw_json_text    = JSON:encode(lua_table_or_value)
--   local pretty_json_text = JSON:encode_pretty(lua_table_or_value) -- "pretty printed" version for human readability
--
--
-- DECODING
-- 
--   JSON = (loadfile "JSON:lua")() -- one-time load of the routines
--
--   local lua_value = JSON:decode(raw_json_text)
--
--   If the JSON text is for an object or an array, e.g.
--     { "what": "books", "count": 3 }
--   or
--     [ "Larry", "Curly", "Moe" ]
--  
--   the result is a Lua table, e.g.
--     { what = "books", count = 3 }
--   or
--     { "Larry", "Curly", "Moe" }
--
--   With most errors during decoding, this code calls
--  
--      JSON:onDecodeError(message, [text, [location]])
--
--   with a message about the error, and if known, the JSON text being parsed
--   and the byte count where the problem was discovered. You can replace the
--   default JSON:onDecodeError() with your own function. The default merely augments
--   the message with data about the text and the location, if known, and then
--   calls JSON.assert(), which itself defaults to Lua's built-in assert().
--
--   For example, in a Lightroom plugin, you might use something like
--
--          function JSON:onDecodeError(message, text, location)
--             LrErrors.throwUserError("Internal Error: invalid JSON data")
--          end
--
--   If JSON:decode() is passed a nil, this is called instead:
--
--      JSON:onDecodeOfNilError(message)
--  
--   and if JSON:decode() is passed HTML instead of JSON, this is called:
--
--      JSON:onDecodeOfHTMLError(message, text)
--
-- DECODING AND STRICT TYPES
--   
--   Because both JSON objects and JSON arrays are converted to Lua tables, it's not normally
--   possible to tell which a Lua table came from, or guarantee decode-encode round-trip
--   equivalency.
--
--   However, if you enable strictTypes, e.g.
--
--      JSON = (loadfile "JSON:lua")() --load the routines
--      JSON.strictTypes = true
--
--   then the Lua table resulting from the decoding of a JSON object or JSON array
--   is marked via Lua metatable, so that when re-encoded with JSON:encode() it ends
--   up as the appropriate JSON type.
--
--   (This is not the default because other routines may not work well with tables that
--   have a metatable set, for example, Lightroom API calls.)
--
--
-- ENCODING
--
--   JSON = (loadfile "JSON.lua")() -- one-time load of the routines
--
--   local raw_json_text    = JSON:encode(lua_table_or_value)
--   local pretty_json_text = JSON:encode_pretty(lua_table_or_value) -- "pretty printed" version for human readability
  
--   On error during encoding, this code calls:
--
--    JSON:onEncodeError(message)
--
--   which you can override in your local JSON object.
--
--
-- SUMMARY OF METHODS YOU CAN OVERRIDE IN YOUR LOCAL LUA JSON OBJECT
--
--    assert
--    onDecodeError
--    onDecodeOfNilError
--    onDecodeOfHTMLError
--    onEncodeError
--
--  If you want to create a separate Lua JSON object with its own error handlers,
--  you can reload JSON.lua or use the :new() method.
--
---------------------------------------------------------------------------


local isArray  = { __tostring = function() return "JSON array"  end }    isArray.__index  = isArray
local isObject = { __tostring = function() return "JSON object" end }    isObject.__index = isObject

function OBJDEF:newArray(tbl)
   return setmetatable(tbl or {}, isArray)
end 

function OBJDEF:newObject(tbl)
   return setmetatable(tbl or {}, isObject)
end 

local function unicode_codepoint_as_utf8(codepoint)
   --
   -- codepoint is a number
   --
   if codepoint <= 127 then
      return string.char(codepoint)

   elseif codepoint <= 2047 then
      --
      -- 110yyyxx 10xxxxxx         <-- useful notation from http://en.wikipedia.org/wiki/Utf8
      --
      local highpart = math.floor(codepoint / 0x40)
      local lowpart  = codepoint - (0x40 * highpart)
      return string.char(0xC0 + highpart,
                         0x80 + lowpart)

   elseif codepoint <= 65535 then
      --
      -- 1110yyyy 10yyyyxx 10xxxxxx
      --
      local highpart  = math.floor(codepoint / 0x1000)
      local remainder = codepoint - 0x1000 * highpart
      local midpart   = math.floor(remainder / 0x40)
      local lowpart   = remainder - 0x40 * midpart

      highpart = 0xE0 + highpart
      midpart  = 0x80 + midpart
      lowpart  = 0x80 + lowpart
               
      --
      -- Check for an invalid characgter (thanks Andy R. at Adobe).
      -- See table 3.7, page 93, in http://www.unicode.org/versions/Unicode5.2.0/ch03.pdf#G28070
      --
      if ( highpart == 0xE0 and midpart < 0xA0 ) or
         ( highpart == 0xED and midpart > 0x9F ) or
         ( highpart == 0xF0 and midpart < 0x90 ) or
         ( highpart == 0xF4 and midpart > 0x8F )
      then
         return "?"
      else
         return string.char(highpart,
                            midpart,
                            lowpart)
      end

   else
      --
      -- Not actually used in this JSON-parsing code, but included here for completeness.
      --
      -- 11110zzz 10zzyyyy 10yyyyxx 10xxxxxx
      --
      local highpart  = math.floor(codepoint / 0x40000)
      local remainder = codepoint - 0x40000 * highpart
      local midA      = math.floor(remainder / 0x1000)
      remainder       = remainder - 0x1000 * midA
      local midB      = math.floor(remainder / 0x40)
      local lowpart   = remainder - 0x40 * midB

      return string.char(0xF0 + highpart,
                         0x80 + midA,
                         0x80 + midB,
                         0x80 + lowpart)
   end
end

function OBJDEF:onDecodeError(message, text, location)
   if text then
      if location then
         message = string.format("%s at char %d of: %s", message, location, text)
      else
         message = string.format("%s: %s", message, text)
      end
   end
   if self.assert then
      self.assert(false, message)
   else
      assert(false, message)
   end
end

OBJDEF.onDecodeOfNilError  = OBJDEF.onDecodeError
OBJDEF.onDecodeOfHTMLError = OBJDEF.onDecodeError

function OBJDEF:onEncodeError(message)
   if self.assert then
      self.assert(false, message)
   else
      assert(false, message)
   end
end

local function grok_number(self, text, start)
   --
   -- Grab the integer part
   --
   local integer_part = text:match('^-?[1-9]%d*', start)
                     or text:match("^-?0",        start)

   if not integer_part then
      self:onDecodeError("expected number", text, start)
   end

   local i = start + integer_part:len()

   --
   -- Grab an optional decimal part
   --
   local decimal_part = text:match('^%.%d+', i) or ""

   i = i + decimal_part:len()

   --
   -- Grab an optional exponential part
   --
   local exponent_part = text:match('^[eE][-+]?%d+', i) or ""

   i = i + exponent_part:len()

   local full_number_text = integer_part .. decimal_part .. exponent_part
   local as_number = tonumber(full_number_text)

   if not as_number then
      self:onDecodeError("bad number", text, start)
   end

   return as_number, i
end

local function grok_string(self, text, start)

   if text:sub(start,start) ~= '"' then
      self:onDecodeError("expected string's opening quote", text, start)
   end

   local i = start + 1 -- +1 to bypass the initial quote
   local text_len = text:len()
   local VALUE = ""
   while i <= text_len do
      local c = text:sub(i,i)
      if c == '"' then
         return VALUE, i + 1
      end
      if c ~= '\\' then
         VALUE = VALUE .. c
         i = i + 1
      elseif text:match('^\\b', i) then
         VALUE = VALUE .. "\b"
         i = i + 2
      elseif text:match('^\\f', i) then
         VALUE = VALUE .. "\f"
         i = i + 2
      elseif text:match('^\\n', i) then
         VALUE = VALUE .. "\n"
         i = i + 2
      elseif text:match('^\\r', i) then
         VALUE = VALUE .. "\r"
         i = i + 2
      elseif text:match('^\\t', i) then
         VALUE = VALUE .. "\t"
         i = i + 2
      elseif text:match('^\\u', i) then
         local hex = text:match('^\\u([0123456789aAbBcCdDeEfF][0123456789aAbBcCdDeEfF][0123456789aAbBcCdDeEfF][0123456789aAbBcCdDeEfF])', i)
         VALUE = VALUE .. unicode_codepoint_as_utf8(tonumber(hex, 16))
         i = i + 6
      else
         -- just pass through what's escaped
         VALUE = VALUE .. text:match('^\\(.)', i)
         i = i + 2
      end
   end

   self:onDecodeError("unclosed string", text, start)
end

local function skip_whitespace(text, start)
   
   local match_start, match_end = text:find("^[ \n\r\t]+", start) -- [http://www.ietf.org/rfc/rfc4627.txt] Section 2
   if match_end then
      return match_end + 1
   else
      return start
   end
end

local grok_one -- assigned later

local function grok_object(self, text, start)
   if not text:sub(start,start) == '{' then
      self:onDecodeError("expected '{'", text, start)
   end

   local i = skip_whitespace(text, start + 1) -- +1 to skip the '{'

   local VALUE = self.strictTypes and self:newObject { } or { }

   if text:sub(i,i) == '}' then
      return VALUE, i + 1
   end
   local text_len = text:len()
   while i <= text_len do
      local key, new_i = grok_string(self, text, i)

      i = skip_whitespace(text, new_i)

      if text:sub(i, i) ~= ':' then
         self:onDecodeError("expected colon", text, i)
      end

      i = skip_whitespace(text, i + 1)

      local val, new_i = grok_one(self, text, i)

      VALUE[key] = val

      --
      -- Expect now either '}' to end things, or a ',' to allow us to continue.
      --
      i = skip_whitespace(text, new_i)

      local c = text:sub(i,i)

      if c == '}' then
         return VALUE, i + 1
      end

      if text:sub(i, i) ~= ',' then
         self:onDecodeError("expected comma or '}'", text, i)
      end

      i = skip_whitespace(text, i + 1)
   end

   self:onDecodeError("unclosed '{'", text, start)
end

local function grok_array(self, text, start)
   if not text:sub(start,start) == '[' then
      self:onDecodeError("expected '['", text, start)
   end

   local i = skip_whitespace(text, start + 1) -- +1 to skip the '['
   local VALUE = self.strictTypes and self:newArray { } or { }
   if text:sub(i,i) == ']' then
      return VALUE, i + 1
   end

   local text_len = text:len()
   while i <= text_len do
      local val, new_i = grok_one(self, text, i)

      table.insert(VALUE, val)

      i = skip_whitespace(text, new_i)

      --
      -- Expect now either ']' to end things, or a ',' to allow us to continue.
      --
      local c = text:sub(i,i)
      if c == ']' then
         return VALUE, i + 1
      end
      if text:sub(i, i) ~= ',' then
         self:onDecodeError("expected comma or '['", text, i)
      end
      i = skip_whitespace(text, i + 1)
   end
   self:onDecodeError("unclosed '['", text, start)
end


grok_one = function(self, text, start)
   -- Skip any whitespace
   start = skip_whitespace(text, start)

   if start > text:len() then
      self:onDecodeError("unexpected end of string", text)
   end

   if text:find('^"', start) then
      return grok_string(self, text, start)

   elseif text:find('^[-0123456789 ]', start) then
      return grok_number(self, text, start)

   elseif text:find('^%{', start) then
      return grok_object(self, text, start)

   elseif text:find('^%[', start) then
      return grok_array(self, text, start)

   elseif text:find('^true', start) then
      return true, start + 4

   elseif text:find('^false', start) then
      return false, start + 5

   elseif text:find('^null', start) then
      return nil, start + 4

   else
      self:onDecodeError("can't parse JSON", text, start)
   end
end

function OBJDEF:decode(text)
   if type(self) ~= 'table' or self.__index ~= OBJDEF then
      OBJDEF:onDecodeError("JSON:decode must be called in method format")
   end

   if text == nil then
      self:onDecodeOfNilError(string.format("nil passed to JSON:decode()"))
   elseif type(text) ~= 'string' then
      self:onDecodeError(string.format("expected string argument to JSON:decode(), got %s", type(text)))
   end

   if text:match('^%s*$') then
      return nil
   end

   if text:match('^%s*<') then
      -- Can't be JSON... we'll assume it's HTML
      self:onDecodeOfHTMLError(string.format("html passed to JSON:decode()"), text)
   end

   --
   -- Ensure that it's not UTF-32 or UTF-16
   -- Those are perfectly valid encodings for JSON (as per RFC 4627 section 3),
   -- but this package can't handle them.
   --
   if text:sub(1,1):byte() == 0 or (text:len() >= 2 and text:sub(2,2):byte() == 0) then
      self:onDecodeError("JSON package groks only UTF-8, sorry", text)
   end

   local success, value = pcall(grok_one, self, text, 1)
   if success then
      return value
   else
      assert(false, value)
      return nil
   end
end

local function backslash_replacement_function(c)
   if c == "\n" then
      return "\\n"
   elseif c == "\r" then
      return "\\r"
   elseif c == "\t" then
      return "\\t"
   elseif c == "\b" then
      return "\\b"
   elseif c == "\f" then
      return "\\f"
   elseif c == '"' then
      return '\\"'
   elseif c == '\\' then
      return '\\\\'
   else
      return string.format("\\u%04x", c:byte())
   end
end

local chars_to_be_escaped_in_JSON_string
   = '['
   ..    '"'    -- class sub-pattern to match a double quote
   ..    '%\\'  -- class sub-pattern to match a backslash
   ..    '%z'   -- class sub-pattern to match a null
   ..    '\001' .. '-' .. '\031' -- class sub-pattern to match control characters
   .. ']'

local function json_string_literal(value)
   local newval = value:gsub(chars_to_be_escaped_in_JSON_string, backslash_replacement_function)
   return '"' .. newval .. '"'
end

local function object_or_array(self, T)
   --
   -- We need to inspect all the keys... if there are any strings, we'll convert to a JSON
   -- object. If there are only numbers, it's a JSON array.
   --
   -- If we'll be converting to a JSON object, we'll want to sort the keys so that the
   -- end result is deterministic.
   --
   local keys = { }
   local seen_number_key = false

   for key in pairs(T) do
      if type(key) == 'number' then
         seen_number_key = true
      elseif type(key) == 'string' then
         table.insert(keys, key)
      else
         self:onEncodeError("can't encode table with a key of type " .. type(key))
      end
   end

   if seen_number_key and #keys > 0 then
      --
      -- Mixed key types... don't know what to do, so bail
      --
      self:onEncodeError("a table with both numeric and string keys could be an object or array; aborting")

   elseif #keys == 0  then
      if seen_number_key then
         return nil -- an array
      else
         --
         -- An empty table...
         --
         if tostring(T) == "JSON array" then
            return nil
         elseif tostring(T) == "JSON object" then
            return { }
         else
            -- have to guess, so we'll pick array, since empty arrays are likely more common than empty objects
            return nil
         end
      end
   else
      --
      -- An object, so return a list of keys
      --
      table.sort(keys)
      return keys
   end
end

--
-- Encode
--
local encode_value -- must predeclare because it calls itself
function encode_value(self, value, parents)

   if type(value) == 'string' then
      return json_string_literal(value)

   elseif type(value) == 'number' then
      return tostring(value)

   elseif type(value) == 'boolean' then
      return tostring(value)

   elseif type(value) == 'nil' then
      return 'null'

   elseif type(value) ~= 'table' then
      self:onEncodeError("can't convert " .. type(value) .. " to JSON")

   else
      --
      -- A table to be converted to either a JSON object or array.
      --
      local T = value

      if parents[T] then
         self:onEncodeError("table " .. tostring(T) .. " is a child of itself")
      else
         parents[T] = true
      end

      local result_value

      local object_keys = object_or_array(self, T)
      if not object_keys then
         --
         -- An array...  
         --
         local ITEMS = { }
         for i = 1, #T do
            table.insert(ITEMS, encode_value(self, T[i], parents))
         end

         result_value = "[" .. table.concat(ITEMS, ",") .. "]"

      else

         --
         -- An object -- can keys be numbers?
         --

         --
         -- We'll always sort the keys, so that comparisons can be made on
         -- the results, etc. The actual order is not particularly
         -- important (e.g. it doesn't matter what character set we sort
         -- as); it's only important that it be deterministic... the same
         -- every time.
         --
         local PARTS = { }
         for _, key in ipairs(object_keys) do
            local encoded_key = encode_value(self, tostring(key), parents)
            local encoded_val = encode_value(self, T[key],        parents)
            table.insert(PARTS, string.format("%s:%s", encoded_key, encoded_val))
         end
         result_value = "{" .. table.concat(PARTS, ",") .. "}"
      end

      parents[T] = false
      return result_value
   end
end

local encode_pretty_value -- must predeclare because it calls itself
function encode_pretty_value(self, value, parents, indent)

   if type(value) == 'string' then
      return json_string_literal(value)

   elseif type(value) == 'number' then
      return tostring(value)

   elseif type(value) == 'boolean' then
      return tostring(value)

   elseif type(value) == 'nil' then
      return 'null'

   elseif type(value) ~= 'table' then
      self:onEncodeError("can't convert " .. type(value) .. " to JSON")

   else
      --
      -- A table to be converted to either a JSON object or array.
      --
      local T = value

      if parents[T] then
         self:onEncodeError("table " .. tostring(T) .. " is a child of itself")
      end
      parents[T] = true
     
      local result_value

      local object_keys = object_or_array(self, T)
      if not object_keys then
         --
         -- An array...  
         --
         local ITEMS = { }
         for i = 1, #T do
            table.insert(ITEMS, encode_pretty_value(self, T[i], parents, indent))
         end

         result_value = "[ " .. table.concat(ITEMS, ", ") .. " ]"

      else

         --
         -- An object -- can keys be numbers?
         --

         local KEYS = { }
         local max_key_length = 0
         for _, key in ipairs(object_keys) do
            local encoded = encode_pretty_value(self, tostring(key), parents, "")
            max_key_length = math.max(max_key_length, #encoded)
            table.insert(KEYS, encoded)
         end
         local key_indent = indent .. "    "
         local subtable_indent = indent .. string.rep(" ", max_key_length + 2 + 4)
         local FORMAT = "%s%" .. tostring(max_key_length) .. "s: %s"

         local COMBINED_PARTS = { }
         for i, key in ipairs(object_keys) do
            local encoded_val = encode_pretty_value(self, T[key], parents, subtable_indent)
            table.insert(COMBINED_PARTS, string.format(FORMAT, key_indent, KEYS[i], encoded_val))
         end
         result_value = "{\n" .. table.concat(COMBINED_PARTS, ",\n") .. "\n" .. indent .. "}"
      end

      parents[T] = false
      return result_value
   end
end

function OBJDEF:encode(value)
   if type(self) ~= 'table' or self.__index ~= OBJDEF then
      OBJDEF:onEncodeError("JSON:encode must be called in method format")
   end

   local parents = {}
   return encode_value(self, value, parents)
end

function OBJDEF:encode_pretty(value)
   local parents = {}
   local subtable_indent = ""
   return encode_pretty_value(self, value, parents, subtable_indent)
end

function OBJDEF.__tostring()
   return "JSON encode/decode package"
end

OBJDEF.__index = OBJDEF

function OBJDEF:new(args)
   local new = { }

   if args then
      for key, val in pairs(args) do
         new[key] = val
      end
   end

   return setmetatable(new, OBJDEF)
end


function RegisterObject()
	local obj = {
		decode = function(self, text)
			local o = OBJDEF:new()
			return o:decode(text)
		end,
		encode = function(self, value)
			local o = OBJDEF:new()
			return o:encode(value)
		end,
	}
	
	XLSetGlobal("WE.Json", obj)
end

RegisterObject()

--
-- Version history:
--
--   20100810.2    added some checking to ensure that an invalid Unicode character couldn't leak in to the UTF-8 encoding
--
--   20100731.1    initial public release
--
