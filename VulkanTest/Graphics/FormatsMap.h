#pragma once
#include "../Workspace.h"
/* Copyright (c) 2015-2019 The Khronos Group Inc.
 * Copyright (c) 2015-2019 Valve Corporation
 * Copyright (c) 2015-2019 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Mark Lobodzinski <mark@lunarg.com>
 * Author: Dave Houlton <daveh@lunarg.com>
 *
 */
#include <string.h>
#include <string>
#include <vector>
#include <map>

struct VULKAN_FORMAT_INFO {
    uint32_t size;
    uint32_t channel_count;
};
// Disable auto-formatting for this large table
// clang-format off
// Set up data structure with size(bytes) and number of channels for each Vulkan format
// For compressed and multi-plane formats, size is bytes per compressed or shared block
const std::map<VkFormat, VULKAN_FORMAT_INFO> vk_format_table = 
{
    {VK_FORMAT_UNDEFINED,                   {0, 0}},
    {VK_FORMAT_R4G4_UNORM_PACK8,            {1, 2}},
    {VK_FORMAT_R4G4B4A4_UNORM_PACK16,       {2, 4}},
    {VK_FORMAT_B4G4R4A4_UNORM_PACK16,       {2, 4}},
    {VK_FORMAT_R5G6B5_UNORM_PACK16,         {2, 3}},
    {VK_FORMAT_B5G6R5_UNORM_PACK16,         {2, 3}},
    {VK_FORMAT_R5G5B5A1_UNORM_PACK16,       {2, 4}},
    {VK_FORMAT_B5G5R5A1_UNORM_PACK16,       {2, 4}},
    {VK_FORMAT_A1R5G5B5_UNORM_PACK16,       {2, 4}},
    {VK_FORMAT_R8_UNORM,                    {1, 1}},
    {VK_FORMAT_R8_SNORM,                    {1, 1}},
    {VK_FORMAT_R8_USCALED,                  {1, 1}},
    {VK_FORMAT_R8_SSCALED,                  {1, 1}},
    {VK_FORMAT_R8_UINT,                     {1, 1}},
    {VK_FORMAT_R8_SINT,                     {1, 1}},
    {VK_FORMAT_R8_SRGB,                     {1, 1}},
    {VK_FORMAT_R8G8_UNORM,                  {2, 2}},
    {VK_FORMAT_R8G8_SNORM,                  {2, 2}},
    {VK_FORMAT_R8G8_USCALED,                {2, 2}},
    {VK_FORMAT_R8G8_SSCALED,                {2, 2}},
    {VK_FORMAT_R8G8_UINT,                   {2, 2}},
    {VK_FORMAT_R8G8_SINT,                   {2, 2}},
    {VK_FORMAT_R8G8_SRGB,                   {2, 2}},
    {VK_FORMAT_R8G8B8_UNORM,                {3, 3}},
    {VK_FORMAT_R8G8B8_SNORM,                {3, 3}},
    {VK_FORMAT_R8G8B8_USCALED,              {3, 3}},
    {VK_FORMAT_R8G8B8_SSCALED,              {3, 3}},
    {VK_FORMAT_R8G8B8_UINT,                 {3, 3}},
    {VK_FORMAT_R8G8B8_SINT,                 {3, 3}},
    {VK_FORMAT_R8G8B8_SRGB,                 {3, 3}},
    {VK_FORMAT_B8G8R8_UNORM,                {3, 3}},
    {VK_FORMAT_B8G8R8_SNORM,                {3, 3}},
    {VK_FORMAT_B8G8R8_USCALED,              {3, 3}},
    {VK_FORMAT_B8G8R8_SSCALED,              {3, 3}},
    {VK_FORMAT_B8G8R8_UINT,                 {3, 3}},
    {VK_FORMAT_B8G8R8_SINT,                 {3, 3}},
    {VK_FORMAT_B8G8R8_SRGB,                 {3, 3}},
    {VK_FORMAT_R8G8B8A8_UNORM,              {4, 4}},
    {VK_FORMAT_R8G8B8A8_SNORM,              {4, 4}},
    {VK_FORMAT_R8G8B8A8_USCALED,            {4, 4}},
    {VK_FORMAT_R8G8B8A8_SSCALED,            {4, 4}},
    {VK_FORMAT_R8G8B8A8_UINT,               {4, 4}},
    {VK_FORMAT_R8G8B8A8_SINT,               {4, 4}},
    {VK_FORMAT_R8G8B8A8_SRGB,               {4, 4}},
    {VK_FORMAT_B8G8R8A8_UNORM,              {4, 4}},
    {VK_FORMAT_B8G8R8A8_SNORM,              {4, 4}},
    {VK_FORMAT_B8G8R8A8_USCALED,            {4, 4}},
    {VK_FORMAT_B8G8R8A8_SSCALED,            {4, 4}},
    {VK_FORMAT_B8G8R8A8_UINT,               {4, 4}},
    {VK_FORMAT_B8G8R8A8_SINT,               {4, 4}},
    {VK_FORMAT_B8G8R8A8_SRGB,               {4, 4}},
    {VK_FORMAT_A8B8G8R8_UNORM_PACK32,       {4, 4}},
    {VK_FORMAT_A8B8G8R8_SNORM_PACK32,       {4, 4}},
    {VK_FORMAT_A8B8G8R8_USCALED_PACK32,     {4, 4}},
    {VK_FORMAT_A8B8G8R8_SSCALED_PACK32,     {4, 4}},
    {VK_FORMAT_A8B8G8R8_UINT_PACK32,        {4, 4}},
    {VK_FORMAT_A8B8G8R8_SINT_PACK32,        {4, 4}},
    {VK_FORMAT_A8B8G8R8_SRGB_PACK32,        {4, 4}},
    {VK_FORMAT_A2R10G10B10_UNORM_PACK32,    {4, 4}},
    {VK_FORMAT_A2R10G10B10_SNORM_PACK32,    {4, 4}},
    {VK_FORMAT_A2R10G10B10_USCALED_PACK32,  {4, 4}},
    {VK_FORMAT_A2R10G10B10_SSCALED_PACK32,  {4, 4}},
    {VK_FORMAT_A2R10G10B10_UINT_PACK32,     {4, 4}},
    {VK_FORMAT_A2R10G10B10_SINT_PACK32,     {4, 4}},
    {VK_FORMAT_A2B10G10R10_UNORM_PACK32,    {4, 4}},
    {VK_FORMAT_A2B10G10R10_SNORM_PACK32,    {4, 4}},
    {VK_FORMAT_A2B10G10R10_USCALED_PACK32,  {4, 4}},
    {VK_FORMAT_A2B10G10R10_SSCALED_PACK32,  {4, 4}},
    {VK_FORMAT_A2B10G10R10_UINT_PACK32,     {4, 4}},
    {VK_FORMAT_A2B10G10R10_SINT_PACK32,     {4, 4}},
    {VK_FORMAT_R16_UNORM,                   {2, 1}},
    {VK_FORMAT_R16_SNORM,                   {2, 1}},
    {VK_FORMAT_R16_USCALED,                 {2, 1}},
    {VK_FORMAT_R16_SSCALED,                 {2, 1}},
    {VK_FORMAT_R16_UINT,                    {2, 1}},
    {VK_FORMAT_R16_SINT,                    {2, 1}},
    {VK_FORMAT_R16_SFLOAT,                  {2, 1}},
    {VK_FORMAT_R16G16_UNORM,                {4, 2}},
    {VK_FORMAT_R16G16_SNORM,                {4, 2}},
    {VK_FORMAT_R16G16_USCALED,              {4, 2}},
    {VK_FORMAT_R16G16_SSCALED,              {4, 2}},
    {VK_FORMAT_R16G16_UINT,                 {4, 2}},
    {VK_FORMAT_R16G16_SINT,                 {4, 2}},
    {VK_FORMAT_R16G16_SFLOAT,               {4, 2}},
    {VK_FORMAT_R16G16B16_UNORM,             {6, 3}},
    {VK_FORMAT_R16G16B16_SNORM,             {6, 3}},
    {VK_FORMAT_R16G16B16_USCALED,           {6, 3}},
    {VK_FORMAT_R16G16B16_SSCALED,           {6, 3}},
    {VK_FORMAT_R16G16B16_UINT,              {6, 3}},
    {VK_FORMAT_R16G16B16_SINT,              {6, 3}},
    {VK_FORMAT_R16G16B16_SFLOAT,            {6, 3}},
    {VK_FORMAT_R16G16B16A16_UNORM,          {8, 4}},
    {VK_FORMAT_R16G16B16A16_SNORM,          {8, 4}},
    {VK_FORMAT_R16G16B16A16_USCALED,        {8, 4}},
    {VK_FORMAT_R16G16B16A16_SSCALED,        {8, 4}},
    {VK_FORMAT_R16G16B16A16_UINT,           {8, 4}},
    {VK_FORMAT_R16G16B16A16_SINT,           {8, 4}},
    {VK_FORMAT_R16G16B16A16_SFLOAT,         {8, 4}},
    {VK_FORMAT_R32_UINT,                    {4, 1}},
    {VK_FORMAT_R32_SINT,                    {4, 1}},
    {VK_FORMAT_R32_SFLOAT,                  {4, 1}},
    {VK_FORMAT_R32G32_UINT,                 {8, 2}},
    {VK_FORMAT_R32G32_SINT,                 {8, 2}},
    {VK_FORMAT_R32G32_SFLOAT,               {8, 2}},
    {VK_FORMAT_R32G32B32_UINT,              {12, 3}},
    {VK_FORMAT_R32G32B32_SINT,              {12, 3}},
    {VK_FORMAT_R32G32B32_SFLOAT,            {12, 3}},
    {VK_FORMAT_R32G32B32A32_UINT,           {16, 4}},
    {VK_FORMAT_R32G32B32A32_SINT,           {16, 4}},
    {VK_FORMAT_R32G32B32A32_SFLOAT,         {16, 4}},
    {VK_FORMAT_R64_UINT,                    {8, 1}},
    {VK_FORMAT_R64_SINT,                    {8, 1}},
    {VK_FORMAT_R64_SFLOAT,                  {8, 1}},
    {VK_FORMAT_R64G64_UINT,                 {16, 2}},
    {VK_FORMAT_R64G64_SINT,                 {16, 2}},
    {VK_FORMAT_R64G64_SFLOAT,               {16, 2}},
    {VK_FORMAT_R64G64B64_UINT,              {24, 3}},
    {VK_FORMAT_R64G64B64_SINT,              {24, 3}},
    {VK_FORMAT_R64G64B64_SFLOAT,            {24, 3}},
    {VK_FORMAT_R64G64B64A64_UINT,           {32, 4}},
    {VK_FORMAT_R64G64B64A64_SINT,           {32, 4}},
    {VK_FORMAT_R64G64B64A64_SFLOAT,         {32, 4}},
    {VK_FORMAT_B10G11R11_UFLOAT_PACK32,     {4, 3}},
    {VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,      {4, 3}},
    {VK_FORMAT_D16_UNORM,                   {2, 1}},
    {VK_FORMAT_X8_D24_UNORM_PACK32,         {4, 1}},
    {VK_FORMAT_D32_SFLOAT,                  {4, 1}},
    {VK_FORMAT_S8_UINT,                     {1, 1}},
    {VK_FORMAT_D16_UNORM_S8_UINT,           {3, 2}},
    {VK_FORMAT_D24_UNORM_S8_UINT,           {4, 2}},
    {VK_FORMAT_D32_SFLOAT_S8_UINT,          {8, 2}}
};

static bool IsDepthFormat(VkFormat format_)
{
    return (VK_FORMAT_D16_UNORM <= (Uint)format_) && ((Uint)format_ <= VK_FORMAT_D32_SFLOAT_S8_UINT);
}

static bool IsColorFormat(VkFormat format_)
{
    Bool range0 = (VK_FORMAT_R8_UNORM <= format_) && (format_ <= VK_FORMAT_B8G8R8A8_SRGB);
    Bool range1 = (VK_FORMAT_R16_UNORM <= format_) && (format_ <= VK_FORMAT_R64G64B64A64_SFLOAT);
    
    return (range0 || range1);
}