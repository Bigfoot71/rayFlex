#include "raylib.h"
#include <rayflex.hpp>

using namespace rf;

constexpr uint8_t map[] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x08, 0x03, 0x00, 0x00, 0x00, 0xF4, 0xE0, 0x91, 
    0xF9, 0x00, 0x00, 0x00, 0x36, 0x50, 0x4C, 0x54, 0x45, 0xCC, 0xCC, 0xCC, 0xC2, 0xC2, 0xC2, 0xC8, 
    0xC8, 0xC8, 0xB8, 0xB8, 0xB8, 0xD6, 0xD6, 0xD6, 0xBC, 0xBC, 0xBC, 0xD0, 0xD0, 0xD0, 0xC5, 0xC5, 
    0xC5, 0xBE, 0xBE, 0xBE, 0xD2, 0xD2, 0xD2, 0xB4, 0xB4, 0xB4, 0xB0, 0xB0, 0xB0, 0xDB, 0xDB, 0xDB, 
    0xAC, 0xAC, 0xAC, 0xA5, 0xA5, 0xA5, 0xA9, 0xA9, 0xA9, 0xDE, 0xDE, 0xDE, 0xA0, 0xA0, 0xA0, 0x8F, 
    0x1E, 0xAB, 0x94, 0x00, 0x00, 0x0E, 0x6E, 0x49, 0x44, 0x41, 0x54, 0x78, 0xDA, 0x7C, 0x9A, 0x89, 
    0x72, 0x9B, 0x40, 0x0C, 0x86, 0x31, 0x31, 0x87, 0x1D, 0x77, 0xDA, 0xF7, 0x7F, 0xDA, 0x22, 0xE9, 
    0x3F, 0x24, 0x32, 0xAD, 0x92, 0x00, 0x4E, 0x00, 0x7D, 0xAB, 0x6B, 0xB5, 0x3B, 0x59, 0xD6, 0x92, 
    0x47, 0xC8, 0x16, 0x72, 0x6E, 0x5F, 0x5F, 0xEF, 0xF7, 0xFB, 0xBB, 0xCB, 0x9B, 0xE7, 0xAF, 0x4B, 
    0xE2, 0xA6, 0xEB, 0xE6, 0x63, 0xD9, 0xF7, 0xFD, 0x95, 0xB2, 0xC7, 0xD5, 0xF3, 0x92, 0xEB, 0x72, 
    0xA1, 0xAC, 0xEB, 0x32, 0x64, 0x5F, 0xF2, 0xEE, 0xEB, 0xA6, 0x7D, 0xCF, 0x27, 0x96, 0x25, 0xD4, 
    0xFA, 0xF6, 0xF5, 0x38, 0x1E, 0x8F, 0xB3, 0x38, 0x42, 0xCF, 0xDB, 0x52, 0x44, 0x71, 0x71, 0xC9, 
    0x45, 0x78, 0x9E, 0x97, 0xFE, 0x75, 0x29, 0xA5, 0xA9, 0x38, 0xAE, 0x3E, 0x79, 0xB5, 0x97, 0x2C, 
    0x37, 0x01, 0xA3, 0x24, 0xA8, 0x43, 0xED, 0xB1, 0xE8, 0xF6, 0x8B, 0xE0, 0x51, 0x72, 0x26, 0x41, 
    0x6A, 0x4B, 0x9D, 0xC0, 0xE1, 0xE8, 0xA9, 0x7F, 0xD9, 0xFD, 0xBE, 0x04, 0xF8, 0x7C, 0xEA, 0xCD, 
    0x78, 0xF9, 0xD4, 0x6F, 0x00, 0xD8, 0x0A, 0x36, 0x58, 0x60, 0x14, 0x12, 0x1C, 0x40, 0x28, 0x6D, 
    0x5F, 0x79, 0x32, 0x49, 0xA9, 0x27, 0xC0, 0xEB, 0xC9, 0x71, 0xE7, 0x45, 0x01, 0x84, 0x22, 0x7A, 
    0x02, 0x18, 0x3B, 0x00, 0xA8, 0xBC, 0x00, 0x5E, 0x00, 0x68, 0xAE, 0x5B, 0x8F, 0x4B, 0xE0, 0x8A, 
    0x33, 0xA3, 0x21, 0x04, 0xF6, 0xB8, 0x1B, 0x00, 0x1E, 0x08, 0xAD, 0x60, 0x48, 0x80, 0x1C, 0xE9, 
    0xFE, 0x1F, 0x0B, 0xBC, 0xF0, 0x43, 0x13, 0x2C, 0x00, 0x62, 0x20, 0xAC, 0x72, 0x46, 0xE8, 0x39, 
    0xF3, 0x20, 0x80, 0xAD, 0x08, 0x1E, 0xF0, 0x00, 0xC7, 0x3D, 0x01, 0xEE, 0x31, 0x10, 0x77, 0x4A, 
    0x3F, 0x11, 0x4C, 0x00, 0x80, 0xE7, 0xDE, 0x22, 0x57, 0x0C, 0x8A, 0x89, 0x04, 0x80, 0x6C, 0x11, 
    0xA8, 0xF9, 0x56, 0xDA, 0xFD, 0x83, 0x28, 0x0C, 0xA1, 0x7E, 0xAB, 0x5F, 0x6D, 0x00, 0x89, 0xDD, 
    0x20, 0x80, 0xB8, 0x94, 0xB1, 0x60, 0x8C, 0x03, 0x01, 0x91, 0x00, 0xD0, 0x0E, 0x82, 0xE6, 0x01, 
    0xBC, 0x8A, 0xE3, 0x92, 0xFA, 0x55, 0x92, 0x61, 0xD0, 0x01, 0x3E, 0x3D, 0x15, 0x9E, 0xE3, 0xC1, 
    0xCA, 0x6D, 0xBD, 0x01, 0x04, 0x0A, 0xC6, 0x4D, 0x00, 0x4B, 0xC4, 0x20, 0x08, 0x72, 0x28, 0x2F, 
    0xDB, 0x95, 0x46, 0xC4, 0x28, 0xD6, 0x1A, 0x18, 0x42, 0xF6, 0x53, 0x66, 0x23, 0x81, 0x00, 0x4C, 
    0x10, 0x62, 0x47, 0x74, 0x80, 0x90, 0x38, 0xB9, 0x0A, 0x7C, 0x52, 0x90, 0x7B, 0x29, 0x6D, 0xFC, 
    0xF9, 0xFC, 0x11, 0xD6, 0x02, 0x40, 0xD2, 0xC6, 0x77, 0x08, 0xEB, 0xC6, 0x72, 0xCB, 0x0B, 0x44, 
    0xB1, 0x09, 0x0E, 0x54, 0x26, 0x32, 0x84, 0x01, 0x98, 0x85, 0x06, 0x40, 0x98, 0xED, 0xD3, 0x01, 
    0x91, 0x56, 0x99, 0x31, 0x2B, 0x00, 0x14, 0xAD, 0x40, 0xB9, 0x24, 0x0D, 0x39, 0x8D, 0xC7, 0xB3, 
    0xBD, 0x70, 0x6E, 0x4C, 0xC3, 0x40, 0x20, 0xC0, 0xEE, 0x1C, 0xB0, 0x62, 0x83, 0x1F, 0xB0, 0x5F, 
    0x12, 0xC0, 0x04, 0x53, 0xD2, 0x74, 0x8B, 0x9D, 0x27, 0x80, 0xF1, 0x22, 0x00, 0x48, 0x08, 0x20, 
    0xA3, 0x3A, 0x82, 0xE7, 0x43, 0x08, 0xE0, 0xB8, 0x3F, 0xE2, 0x20, 0x80, 0xEF, 0x02, 0x00, 0xE9, 
    0x9F, 0xA3, 0x40, 0x69, 0x2A, 0x1F, 0x50, 0x79, 0x1C, 0x1E, 0xB6, 0x00, 0xCC, 0x88, 0xC7, 0xCA, 
    0xEA, 0x4A, 0x9E, 0xD0, 0x4E, 0x9F, 0x25, 0x01, 0x27, 0xAF, 0x3A, 0x21, 0xF2, 0xC0, 0x05, 0xFD, 
    0xF6, 0xFD, 0x6D, 0x30, 0x11, 0x7C, 0x90, 0x78, 0x1D, 0x0D, 0xF0, 0xAC, 0x08, 0xB4, 0xC3, 0xCA, 
    0xE4, 0x35, 0x9F, 0x40, 0x3F, 0x0A, 0x07, 0x08, 0x2C, 0xAF, 0x54, 0x0B, 0x00, 0x13, 0x4C, 0x00, 
    0x04, 0x52, 0xBD, 0x6F, 0x3B, 0x43, 0x50, 0x05, 0x08, 0x50, 0xD6, 0x33, 0x00, 0x05, 0x45, 0x1C, 
    0x69, 0xAB, 0x38, 0xE0, 0x4C, 0x9C, 0x04, 0xB8, 0x7C, 0x11, 0x60, 0x06, 0x1F, 0x5D, 0xB0, 0x1E, 
    0x35, 0xA2, 0xF8, 0x29, 0xC1, 0xCB, 0x40, 0x2E, 0x80, 0x39, 0x9F, 0xD2, 0x61, 0x00, 0x10, 0x82, 
    0xB4, 0x48, 0x1F, 0x9D, 0x31, 0x5D, 0x10, 0x02, 0x00, 0x7A, 0xC1, 0xB2, 0x1A, 0x80, 0x33, 0x30, 
    0x9E, 0xB2, 0xFE, 0x2E, 0x61, 0xBA, 0x7C, 0xEE, 0x96, 0x64, 0x00, 0x90, 0x0B, 0x10, 0x88, 0x03, 
    0x60, 0x4D, 0x80, 0x89, 0x70, 0x08, 0xC0, 0xA5, 0xB8, 0x5B, 0x20, 0xFC, 0x84, 0xFE, 0x41, 0x99, 
    0x53, 0x0C, 0xAC, 0xCA, 0x13, 0x80, 0x0D, 0x02, 0xF4, 0x1B, 0x60, 0x85, 0x05, 0x08, 0xC0, 0xAC, 
    0x76, 0x61, 0x71, 0x55, 0xED, 0x2D, 0x4D, 0x78, 0x9E, 0x00, 0xEF, 0xB8, 0x20, 0x05, 0x9C, 0x67, 
    0xD9, 0xAB, 0x44, 0x2A, 0x27, 0x86, 0x07, 0x8E, 0x3C, 0xA4, 0xC2, 0xF8, 0x86, 0x88, 0x20, 0x00, 
    0x58, 0xD1, 0xDC, 0xD1, 0xA0, 0x93, 0x40, 0x43, 0x57, 0xC7, 0xE8, 0x24, 0x21, 0xE8, 0x24, 0x38, 
    0x4B, 0xD6, 0xB9, 0xE2, 0xB2, 0xFA, 0xA2, 0x9E, 0xFD, 0x56, 0x7B, 0x97, 0x51, 0x87, 0xE8, 0x40, 
    0x96, 0x4D, 0x44, 0xFE, 0xFB, 0x3B, 0x14, 0xAB, 0x97, 0x7D, 0xC3, 0x12, 0xDB, 0x59, 0x49, 0x5C, 
    0x92, 0xC4, 0x00, 0x08, 0x82, 0x97, 0xAB, 0x78, 0x53, 0x7E, 0xFA, 0x7C, 0xE6, 0x57, 0x4A, 0x3A, 
    0xA1, 0x03, 0xEC, 0x95, 0x85, 0x00, 0xA0, 0xEA, 0x09, 0xF0, 0x2E, 0x3F, 0x08, 0x21, 0x1F, 0x50, 
    0x71, 0xC0, 0x3B, 0xFA, 0x24, 0x02, 0xED, 0x52, 0x8B, 0x13, 0x09, 0x76, 0x4E, 0x62, 0x04, 0x38, 
    0x1E, 0x36, 0x80, 0xE5, 0x17, 0x00, 0xAE, 0xA3, 0x32, 0x82, 0x00, 0x07, 0x00, 0x58, 0xFF, 0x43, 
    0x33, 0x5D, 0xAF, 0xB0, 0x3B, 0x79, 0x30, 0x85, 0xA7, 0xC3, 0x17, 0xA4, 0x3C, 0xE7, 0x8E, 0xFE, 
    0xFB, 0xD7, 0x25, 0x71, 0x08, 0x69, 0x16, 0xA0, 0x0D, 0x18, 0xDB, 0x04, 0x80, 0x30, 0xF4, 0x58, 
    0x79, 0x6C, 0x7D, 0x94, 0x21, 0x5E, 0x57, 0x35, 0xDE, 0x5D, 0xD8, 0x19, 0x83, 0xE7, 0x17, 0x00, 
    0x42, 0x79, 0x1E, 0x7E, 0xFF, 0x0E, 0x13, 0x38, 0x1B, 0x36, 0xC4, 0x01, 0x47, 0xB9, 0x74, 0x91, 
    0x01, 0x24, 0xBC, 0xDC, 0x4E, 0x48, 0x02, 0x94, 0x15, 0x55, 0xCF, 0x3A, 0x40, 0x59, 0xE0, 0xBB, 
    0x2C, 0x10, 0xA7, 0xDF, 0x61, 0x81, 0x40, 0x20, 0x00, 0xC3, 0xE0, 0xD1, 0x00, 0x22, 0xE3, 0xF3, 
    0x00, 0xE9, 0x04, 0x21, 0x1C, 0xBF, 0x9C, 0xB1, 0x29, 0x9D, 0x90, 0x41, 0x70, 0x61, 0x06, 0x41, 
    0x00, 0x4C, 0xA1, 0x72, 0x4D, 0x67, 0xCC, 0xEE, 0x6B, 0xC4, 0x8B, 0xC4, 0xE5, 0x47, 0x93, 0x1A, 
    0xDA, 0x19, 0x20, 0x38, 0x1B, 0xCE, 0x9A, 0x12, 0x01, 0x00, 0x17, 0x30, 0x0A, 0x48, 0x50, 0xA3, 
    0xC6, 0xCA, 0xB2, 0x2D, 0x6B, 0x40, 0xC0, 0x40, 0x68, 0xDA, 0x33, 0xFF, 0x61, 0x02, 0xD9, 0x3E, 
    0x3F, 0x81, 0x08, 0x76, 0xC8, 0x61, 0xB6, 0x39, 0x61, 0xE7, 0x3A, 0x0F, 0x85, 0x48, 0x69, 0xA8, 
    0x35, 0x65, 0xB0, 0x78, 0x3E, 0x47, 0x61, 0x46, 0x20, 0xF4, 0xE1, 0x03, 0x00, 0xE9, 0x70, 0x41, 
    0x54, 0x4B, 0x3B, 0x9C, 0x02, 0x47, 0x08, 0xCE, 0x00, 0x36, 0x15, 0x4C, 0xF0, 0x46, 0x40, 0xC6, 
    0x27, 0x66, 0xA0, 0x00, 0x64, 0x01, 0xC4, 0x11, 0xD5, 0x04, 0x82, 0xBA, 0xC1, 0xB5, 0x70, 0xD8, 
    0xDF, 0xFE, 0x0F, 0x80, 0x0E, 0x14, 0x80, 0x27, 0xA3, 0x77, 0xDA, 0x23, 0x01, 0xD2, 0x82, 0xAD, 
    0xAF, 0x04, 0xC0, 0x0B, 0x26, 0x34, 0x80, 0x6D, 0x32, 0x1A, 0x3D, 0x4F, 0x47, 0x15, 0x9B, 0xD4, 
    0xCF, 0xA5, 0xA5, 0x16, 0x75, 0x58, 0x5B, 0x73, 0x5D, 0xD9, 0xCA, 0x70, 0xF2, 0x9D, 0x32, 0x00, 
    0xA7, 0x35, 0x0E, 0xE1, 0x91, 0x00, 0x83, 0x20, 0xA7, 0x4F, 0x00, 0xF4, 0xC4, 0x40, 0xC6, 0xFE, 
    0x58, 0x48, 0xA4, 0x0A, 0xB5, 0x01, 0x11, 0x11, 0x9E, 0x93, 0xE9, 0xC5, 0x14, 0x03, 0xB4, 0x3C, 
    0xCE, 0xD0, 0x17, 0x41, 0x4C, 0x16, 0xF6, 0xD0, 0x04, 0xA8, 0x15, 0x0F, 0x09, 0xEE, 0x4D, 0xBC, 
    0x9B, 0x00, 0x16, 0xE7, 0x9F, 0x00, 0x72, 0x01, 0x54, 0x68, 0x7F, 0x80, 0x5C, 0x5E, 0xD9, 0x72, 
    0x2F, 0x83, 0xB2, 0x52, 0x0C, 0x00, 0x21, 0x80, 0x08, 0xC2, 0xED, 0xFE, 0x78, 0xB6, 0x48, 0x0E, 
    0x80, 0x1C, 0x54, 0x6B, 0x92, 0xF2, 0x69, 0x2B, 0xA0, 0x07, 0x52, 0x84, 0x57, 0x3E, 0x22, 0xE3, 
    0x04, 0x40, 0x15, 0x6F, 0x56, 0x57, 0x6B, 0xFC, 0x16, 0x80, 0x8B, 0x5C, 0x35, 0xF0, 0xE8, 0x0B, 
    0x04, 0x00, 0xCB, 0xC8, 0x0C, 0xBD, 0x49, 0x0A, 0xCF, 0x1B, 0xD0, 0xDD, 0xB3, 0xA7, 0xD1, 0xFB, 
    0x42, 0x0E, 0x04, 0x88, 0xC9, 0x3E, 0xBD, 0xA8, 0x1C, 0x2F, 0xB6, 0x68, 0x14, 0x52, 0x64, 0x67, 
    0x1C, 0x0C, 0xE0, 0x32, 0xD3, 0x3D, 0x50, 0xD0, 0x82, 0x17, 0xC0, 0x79, 0x03, 0xB8, 0xBE, 0x98, 
    0x06, 0x8E, 0x01, 0xFB, 0x63, 0x79, 0xB8, 0x02, 0x29, 0xBD, 0x95, 0x62, 0x19, 0xA1, 0x06, 0x08, 
    0xB1, 0x85, 0xFE, 0x35, 0x8D, 0x5D, 0xFA, 0xE1, 0x75, 0x4A, 0xA5, 0xE5, 0xC6, 0x8E, 0x35, 0x09, 
    0x26, 0x00, 0x12, 0x7B, 0x02, 0x78, 0x2D, 0x65, 0x27, 0xDC, 0x4C, 0xB0, 0x1A, 0xC0, 0x04, 0xD0, 
    0x69, 0x00, 0xE0, 0x14, 0x40, 0x58, 0x68, 0x02, 0xA0, 0x05, 0xA8, 0xA7, 0xE7, 0xFB, 0xBD, 0x82, 
    0x4A, 0x2D, 0xBC, 0xD1, 0xD3, 0xD6, 0xA8, 0x53, 0x9C, 0xC6, 0x4E, 0x01, 0x5C, 0x6A, 0x04, 0x60, 
    0x0F, 0xD4, 0x46, 0x23, 0xE4, 0x58, 0xF2, 0xB9, 0x2E, 0x2A, 0x01, 0x13, 0x80, 0xBF, 0xB6, 0xE2, 
    0x6E, 0x02, 0x3F, 0x79, 0x14, 0xC0, 0xC6, 0xD6, 0x63, 0x83, 0xF4, 0x24, 0xDC, 0x58, 0x13, 0x14, 
    0x84, 0xA5, 0xD0, 0x2F, 0x6F, 0x16, 0x40, 0xA5, 0xB6, 0xCA, 0x49, 0xD0, 0x32, 0x50, 0x1B, 0x6C, 
    0x9B, 0xA5, 0xE2, 0xAD, 0x2E, 0xB4, 0x6C, 0x4A, 0x61, 0x6F, 0x31, 0xAC, 0x59, 0xA7, 0x5D, 0x57, 
    0xD0, 0x0F, 0x00, 0xC8, 0x34, 0x93, 0x26, 0x01, 0x4F, 0xA3, 0x54, 0x91, 0xA7, 0xAA, 0x39, 0xD7, 
    0xD1, 0xA2, 0x3F, 0xA4, 0xE4, 0x6B, 0xA8, 0xD8, 0x89, 0xBF, 0x5A, 0xBF, 0xFA, 0x0D, 0xD3, 0x11, 
    0xDA, 0xE4, 0x29, 0x2A, 0xC6, 0xD0, 0x80, 0x81, 0x4A, 0xB4, 0x71, 0x40, 0x02, 0x00, 0xD8, 0xDF, 
    0x79, 0xB0, 0x3E, 0xA8, 0xC7, 0x06, 0x80, 0xE0, 0x4C, 0xD0, 0x4F, 0x3D, 0x11, 0xD3, 0xEE, 0xE7, 
    0x74, 0x7C, 0x6B, 0xEE, 0x78, 0x43, 0xC6, 0x7C, 0x00, 0xE8, 0xE5, 0xDC, 0xBA, 0xE0, 0x76, 0x99, 
    0x77, 0x4F, 0x06, 0x00, 0x2A, 0x07, 0x74, 0xEB, 0x13, 0x12, 0x29, 0xA5, 0x08, 0x9A, 0xE5, 0x51, 
    0x3F, 0x0E, 0xED, 0xBE, 0x9E, 0x27, 0x7A, 0xAA, 0x8C, 0x9D, 0x1C, 0x7B, 0xDF, 0x6D, 0x24, 0xCE, 
    0x1D, 0x80, 0xED, 0x6F, 0xA8, 0xCC, 0x0A, 0x52, 0xD7, 0xEC, 0xD4, 0xF5, 0x7E, 0xC4, 0x5D, 0x73, 
    0x7D, 0x22, 0xBA, 0x0C, 0xC7, 0x0F, 0xC6, 0x30, 0x37, 0x52, 0xB5, 0xEB, 0x41, 0x1C, 0x22, 0xA8, 
    0x28, 0x05, 0x00, 0x4A, 0xA2, 0xC6, 0x4F, 0x1D, 0x04, 0xD0, 0xC9, 0x13, 0x38, 0x6F, 0x1D, 0x48, 
    0x08, 0x61, 0x0C, 0xF5, 0x23, 0x79, 0x4E, 0x21, 0x10, 0x50, 0xE4, 0x82, 0x10, 0xB4, 0x72, 0x16, 
    0xE9, 0x75, 0xDC, 0x51, 0xBB, 0x78, 0x49, 0x64, 0xEF, 0x97, 0xFE, 0x3F, 0x83, 0x60, 0xD2, 0xD0, 
    0x20, 0xEA, 0x3F, 0xD1, 0x3B, 0x19, 0x00, 0x23, 0xDF, 0x30, 0x7A, 0x0D, 0x3D, 0x8E, 0x34, 0xB6, 
    0xC2, 0x06, 0x9F, 0x4B, 0x34, 0xFE, 0x3F, 0x26, 0xF8, 0x61, 0x0E, 0xF8, 0x63, 0x34, 0x27, 0x32, 
    0x40, 0x8C, 0x1B, 0x99, 0x47, 0xDF, 0x17, 0x42, 0xEF, 0x61, 0xA4, 0x92, 0xFD, 0x04, 0x84, 0x0E, 
    0x28, 0xFD, 0x46, 0x30, 0x84, 0xF5, 0x33, 0x41, 0x0D, 0xE0, 0x56, 0xDC, 0x9B, 0xC9, 0x05, 0xD1, 
    0x32, 0x5F, 0x04, 0xD2, 0x39, 0x08, 0x2A, 0xFC, 0x6C, 0x80, 0x09, 0xF1, 0xEC, 0x21, 0xA1, 0x74, 
    0x80, 0x0B, 0x1C, 0x55, 0x06, 0x60, 0x2B, 0xA8, 0xF1, 0x3F, 0xA0, 0x9B, 0xD3, 0xA7, 0x31, 0xB2, 
    0xF2, 0xC0, 0x03, 0x1F, 0x00, 0xE8, 0x04, 0x28, 0x01, 0x44, 0x08, 0xB8, 0x24, 0x87, 0xDE, 0xA9, 
    0x1F, 0x00, 0x2A, 0x3E, 0xEC, 0x7A, 0x3D, 0xFA, 0x21, 0xDE, 0xB6, 0x84, 0x05, 0x06, 0x80, 0xC6, 
    0x5C, 0x18, 0xCC, 0x44, 0xD8, 0x9F, 0xFA, 0x39, 0x83, 0x4F, 0x80, 0x96, 0x7D, 0xAE, 0x8F, 0x56, 
    0x3A, 0xC7, 0x5F, 0x3B, 0x0B, 0xD0, 0x2F, 0x81, 0x01, 0x24, 0x30, 0x80, 0x6A, 0x20, 0x00, 0x1E, 
    0xD4, 0x7F, 0x0A, 0x00, 0xDA, 0xD5, 0x73, 0xDE, 0x7C, 0x7D, 0x73, 0x01, 0x2A, 0x8F, 0x01, 0xEE, 
    0x69, 0x38, 0x76, 0xD7, 0xFB, 0xFE, 0xDD, 0x04, 0xB0, 0x05, 0x60, 0x7C, 0xE4, 0x3E, 0x15, 0x1B, 
    0xC0, 0x40, 0x13, 0x60, 0xB7, 0x05, 0x0C, 0x70, 0xB7, 0x00, 0xD5, 0x2F, 0xB0, 0x80, 0x63, 0x40, 
    0x04, 0xA5, 0x1F, 0x60, 0x3F, 0x7D, 0x3E, 0x5D, 0xE0, 0xD2, 0x5B, 0x24, 0xB7, 0xCC, 0xC3, 0xF8, 
    0x2D, 0x58, 0xFC, 0x71, 0xB5, 0xFA, 0x3F, 0x80, 0x19, 0x73, 0x75, 0x3F, 0x52, 0xDF, 0xF6, 0xD8, 
    0x43, 0x08, 0x10, 0x2C, 0xD4, 0xEF, 0xCA, 0x0B, 0x02, 0x96, 0xBF, 0xB4, 0x9D, 0x57, 0x25, 0xB5, 
    0x75, 0x51, 0xAE, 0xB8, 0x5B, 0xC0, 0xFA, 0xED, 0x32, 0xF7, 0x0B, 0xBD, 0xD3, 0x01, 0x40, 0x33, 
    0xC1, 0x98, 0x7C, 0x90, 0x78, 0xD8, 0xC1, 0x4C, 0xED, 0x1C, 0x5F, 0xAA, 0x9D, 0xDB, 0x49, 0x9C, 
    0x7C, 0x27, 0x40, 0x5B, 0xC2, 0xB1, 0x6E, 0x37, 0xFD, 0x04, 0xD0, 0xAE, 0xAB, 0x0B, 0xAE, 0x7A, 
    0xB1, 0xBA, 0x19, 0x6E, 0x67, 0x51, 0x3B, 0x04, 0xE0, 0x3C, 0xF4, 0x76, 0x5E, 0x9F, 0x66, 0x3C, 
    0x72, 0xDA, 0x42, 0x11, 0x10, 0x62, 0x00, 0x8E, 0x9C, 0xE3, 0x66, 0x89, 0xDE, 0xF3, 0xAC, 0xD2, 
    0xC7, 0xD2, 0x6A, 0x00, 0xFB, 0x80, 0x2D, 0xB7, 0x76, 0xD3, 0x6D, 0x7A, 0x67, 0x83, 0x0D, 0x20, 
    0x86, 0x22, 0xED, 0xAD, 0x9F, 0x57, 0xFB, 0x38, 0x7B, 0xC6, 0x9B, 0x22, 0x5B, 0x78, 0xFB, 0xA8, 
    0xFC, 0x70, 0xCC, 0x16, 0x11, 0x71, 0x38, 0xBA, 0x6D, 0x23, 0x40, 0x0D, 0xCB, 0x62, 0x0F, 0x5E, 
    0xCB, 0xDD, 0xD8, 0x9A, 0x6A, 0x06, 0x55, 0x03, 0x90, 0xEF, 0x11, 0x37, 0xA1, 0x9E, 0x79, 0x84, 
    0x31, 0x1A, 0x40, 0x89, 0x39, 0x35, 0x8A, 0x81, 0x0E, 0x80, 0x0A, 0x2C, 0x2D, 0xC9, 0x30, 0xF6, 
    0x72, 0x43, 0xD2, 0x34, 0x6E, 0x96, 0xF2, 0xE3, 0x4C, 0x0B, 0x37, 0xC0, 0x74, 0x81, 0xA5, 0x75, 
    0xF7, 0xEA, 0x63, 0xBC, 0x71, 0xC8, 0x35, 0xC6, 0x6D, 0xA5, 0xA3, 0x0B, 0x51, 0x21, 0x60, 0x81, 
    0x2D, 0x82, 0x5D, 0xB6, 0xB5, 0xFE, 0x67, 0x5B, 0x17, 0x68, 0xE5, 0x27, 0xDF, 0x26, 0x05, 0x4A, 
    0xAE, 0xB7, 0x5E, 0xA2, 0xEE, 0x36, 0x80, 0x73, 0x88, 0x71, 0xEC, 0x24, 0xF7, 0x5F, 0xD2, 0x16, 
    0x00, 0xC8, 0x02, 0x8F, 0xB4, 0xF5, 0x6F, 0x70, 0x69, 0x6F, 0x35, 0xF1, 0xDF, 0x35, 0x35, 0xE9, 
    0x69, 0xFB, 0x85, 0xE6, 0xC0, 0xEF, 0xCA, 0x3B, 0xE4, 0x62, 0xC2, 0xCC, 0xA9, 0xC1, 0xB5, 0x08, 
    0x00, 0x6E, 0x9C, 0x70, 0xEB, 0xF5, 0xA5, 0x17, 0xF0, 0x1D, 0x7D, 0x99, 0xED, 0x1D, 0x57, 0x2A, 
    0xF6, 0x2E, 0x10, 0xFE, 0xA8, 0x55, 0x91, 0x72, 0xB7, 0x6F, 0xFF, 0xB9, 0x0F, 0x8B, 0x35, 0xCF, 
    0xDF, 0xBE, 0xCD, 0x40, 0xC9, 0x69, 0x18, 0x06, 0xA2, 0x1D, 0xA0, 0x73, 0x69, 0x99, 0xD2, 0xB9, 
    0xFF, 0xFF, 0x59, 0x2A, 0xF9, 0xED, 0xAE, 0x95, 0x0C, 0x08, 0x86, 0x16, 0xB8, 0x44, 0x2F, 0x92, 
    0x25, 0xCB, 0xB6, 0x72, 0x47, 0x0D, 0xEE, 0x44, 0x23, 0x96, 0xA5, 0xDA, 0x1F, 0xE7, 0x80, 0xD4, 
    0x7C, 0x87, 0x1E, 0x5C, 0xF2, 0x84, 0x0D, 0x80, 0xFA, 0x41, 0xCC, 0x07, 0x00, 0x04, 0xB3, 0xA1, 
    0xC2, 0x00, 0xD1, 0x69, 0xD9, 0x47, 0x54, 0x03, 0xB0, 0xF1, 0xC2, 0xCF, 0x72, 0x32, 0x6A, 0x91, 
    0x77, 0x7E, 0x02, 0x86, 0x05, 0x4E, 0x67, 0x7D, 0x48, 0x52, 0x31, 0x00, 0x0A, 0xAF, 0x59, 0x53, 
    0x60, 0x00, 0x8E, 0x62, 0x54, 0x75, 0x70, 0x01, 0x66, 0xB0, 0x7A, 0xCB, 0x1D, 0x81, 0x5E, 0xFA, 
    0x4F, 0x29, 0xD0, 0xA7, 0xE7, 0x3E, 0xEB, 0x35, 0x3D, 0x1E, 0x09, 0x7A, 0xF9, 0x60, 0x5B, 0x71, 
    0x6D, 0x06, 0xBA, 0x5F, 0x01, 0x62, 0x87, 0xA5, 0x7F, 0x02, 0x3C, 0xA2, 0xFC, 0x0A, 0x40, 0x93, 
    0x48, 0x69, 0x30, 0xBA, 0x6A, 0xEF, 0xAC, 0x3B, 0xD0, 0x0F, 0x04, 0x36, 0x80, 0x21, 0x04, 0x1C, 
    0x53, 0x06, 0xC0, 0x5B, 0xB2, 0xE8, 0x77, 0xB5, 0xBB, 0x01, 0xF8, 0xEA, 0x91, 0x66, 0x9B, 0xA0, 
    0x75, 0xA1, 0x30, 0x07, 0x47, 0xEC, 0xC9, 0xA5, 0xD9, 0x2B, 0x14, 0xB6, 0x80, 0x0A, 0xD4, 0x06, 
    0xA0, 0xD7, 0x04, 0xFD, 0x00, 0xDC, 0xCF, 0xCF, 0x82, 0x09, 0xF6, 0x93, 0xDA, 0x84, 0x45, 0x46, 
    0xE9, 0x04, 0x78, 0xB6, 0xF4, 0x07, 0x0C, 0xF8, 0x8A, 0xB0, 0x5A, 0x0E, 0x60, 0x95, 0xFB, 0x98, 
    0x00, 0xBA, 0x79, 0x00, 0xCE, 0xD5, 0x2D, 0x7D, 0x76, 0xDB, 0x92, 0x3B, 0xFB, 0x3D, 0x57, 0x80, 
    0x38, 0xE2, 0x30, 0x80, 0xBA, 0x48, 0xF6, 0xF1, 0x6F, 0x80, 0x23, 0x04, 0x49, 0x35, 0xD6, 0x12, 
    0x00, 0x75, 0x96, 0x4D, 0x00, 0x12, 0xD1, 0x62, 0x98, 0x63, 0xF1, 0x28, 0x51, 0x6E, 0x73, 0xF8, 
    0xC7, 0x07, 0x25, 0x06, 0x48, 0x61, 0x07, 0x40, 0x21, 0x65, 0x0C, 0x04, 0x20, 0xC6, 0x09, 0x40, 
    0xAC, 0xE7, 0x74, 0x91, 0x8C, 0x45, 0x08, 0x18, 0x20, 0x63, 0xB0, 0x05, 0x17, 0xB8, 0xA0, 0x91, 
    0x74, 0x96, 0xC7, 0x7E, 0x03, 0x00, 0x83, 0xD5, 0xAF, 0xE5, 0x14, 0x68, 0x01, 0x68, 0x21, 0x26, 
    0x8C, 0xEB, 0x32, 0x64, 0x02, 0x60, 0x81, 0x3D, 0xEC, 0x95, 0x8C, 0xD8, 0x4D, 0x75, 0x04, 0x33, 
    0x39, 0x48, 0x7F, 0x62, 0x10, 0x58, 0xBE, 0x5A, 0x6C, 0x02, 0x9F, 0x14, 0x27, 0x0B, 0x44, 0xFD, 
    0x15, 0xE0, 0xD8, 0xEE, 0xD0, 0x08, 0xB1, 0xC0, 0xA8, 0x7C, 0xB9, 0x00, 0xA5, 0x4E, 0x51, 0x71, 
    0x20, 0xA7, 0x65, 0x36, 0x59, 0xD2, 0x90, 0x9B, 0x1F, 0x63, 0x81, 0xBE, 0x67, 0x99, 0x18, 0x00, 
    0xDD, 0x40, 0x0E, 0x67, 0x10, 0x9A, 0x20, 0xB8, 0x8A, 0xCC, 0x09, 0x00, 0x81, 0xC2, 0x01, 0x02, 
    0x03, 0x20, 0x00, 0x54, 0x14, 0x10, 0x2D, 0x76, 0xE9, 0xA9, 0xF9, 0x84, 0xA4, 0x88, 0x2C, 0xA2, 
    0x7C, 0xF5, 0xA5, 0x77, 0x08, 0x50, 0xDA, 0x06, 0xF8, 0x2F, 0x00, 0xFA, 0x0B, 0x80, 0x98, 0x2B, 
    0xB1, 0x7E, 0x5A, 0x68, 0xC7, 0x41, 0x53, 0xD4, 0x52, 0x1F, 0x26, 0x16, 0x45, 0x1E, 0xFA, 0xB4, 
    0x8E, 0xA4, 0x03, 0x93, 0x31, 0x30, 0x53, 0xF1, 0xE3, 0xC6, 0xF1, 0x9A, 0x9D, 0xEF, 0xF6, 0x07, 
    0x8E, 0xFB, 0xCF, 0x33, 0x35, 0x45, 0x86, 0xBE, 0x27, 0x7A, 0x51, 0x9C, 0x27, 0x70, 0x5C, 0x28, 
    0x17, 0x3F, 0x90, 0x5F, 0x12, 0x00, 0x3A, 0x8E, 0x70, 0xFE, 0xE8, 0x69, 0x76, 0x3A, 0x3B, 0x24, 
    0x73, 0x50, 0x50, 0x47, 0x5D, 0xCA, 0x82, 0xBD, 0x05, 0xB5, 0x82, 0x09, 0x00, 0xA5, 0xE2, 0xD3, 
    0x6C, 0xF8, 0x2C, 0x0B, 0x78, 0x00, 0xEA, 0xF9, 0x5F, 0x36, 0xC1, 0xD6, 0x3D, 0x21, 0xFD, 0xAA, 
    0x5F, 0x2F, 0xC7, 0x44, 0xB1, 0xBA, 0x72, 0xBA, 0x9D, 0x56, 0xFA, 0xB5, 0xD1, 0x28, 0x80, 0x9E, 
    0x9B, 0x6E, 0xBF, 0x01, 0xF0, 0x73, 0xE4, 0xD4, 0x1F, 0x0B, 0xC8, 0xF5, 0xB3, 0xBC, 0x5C, 0x1F, 
    0x84, 0xE8, 0x2C, 0x4C, 0x5C, 0xBA, 0xA4, 0xAC, 0x75, 0xE3, 0xD1, 0x65, 0x07, 0xF6, 0x56, 0x0F, 
    0x4A, 0x35, 0x89, 0xF1, 0x76, 0x00, 0xA6, 0x1F, 0xE2, 0x4F, 0x16, 0x88, 0x28, 0x49, 0x66, 0x46, 
    0xC2, 0xEF, 0x99, 0x0F, 0x31, 0x80, 0x2C, 0xC0, 0x6C, 0x90, 0xD6, 0xE2, 0xD7, 0x47, 0x51, 0x5F, 
    0x40, 0xF0, 0x7A, 0x0C, 0x3C, 0x01, 0xE8, 0x80, 0xE7, 0x66, 0x03, 0x20, 0x47, 0x34, 0x8C, 0x1D, 
    0x5D, 0xD5, 0xAE, 0x04, 0x20, 0x16, 0xA0, 0x20, 0x49, 0x51, 0x46, 0x5B, 0x6F, 0x01, 0x20, 0xDD, 
    0x68, 0xD0, 0xD2, 0x5F, 0x48, 0x46, 0x23, 0x06, 0x02, 0xC0, 0x02, 0x18, 0x80, 0x69, 0xB7, 0xA4, 
    0x11, 0x00, 0xD2, 0xEE, 0xE1, 0x55, 0x21, 0x27, 0x31, 0x01, 0xF0, 0xA3, 0x03, 0x80, 0x2D, 0x35, 
    0x40, 0x26, 0x40, 0x84, 0x9A, 0x5D, 0x23, 0x77, 0x75, 0xCC, 0x20, 0x69, 0x17, 0xF1, 0x6C, 0x90, 
    0xBE, 0xD9, 0x06, 0xA8, 0xA5, 0x29, 0xD0, 0x6A, 0xBD, 0x09, 0x00, 0xA5, 0x1D, 0x43, 0x60, 0x7D, 
    0x89, 0xFE, 0x1C, 0xDE, 0x56, 0xCD, 0xFE, 0x2C, 0xF5, 0xEF, 0x16, 0xEE, 0x80, 0x01, 0xE5, 0x06, 
    0x1F, 0x84, 0x5E, 0xCF, 0xA2, 0xE4, 0xED, 0xD2, 0x5F, 0x97, 0xF7, 0xF5, 0x50, 0x15, 0x40, 0x44, 
    0x01, 0x05, 0x41, 0x00, 0x8E, 0xCF, 0xC5, 0xD5, 0x30, 0xF4, 0xA7, 0xA4, 0x6E, 0x31, 0x5F, 0x86, 
    0x98, 0xD1, 0x03, 0x40, 0x8E, 0x86, 0x6C, 0xB2, 0x67, 0x03, 0xBC, 0x16, 0x3F, 0x00, 0x59, 0x7F, 
    0x19, 0x60, 0x1E, 0xED, 0xD3, 0x39, 0xF3, 0x89, 0x5D, 0x00, 0x4C, 0x10, 0x00, 0x57, 0x56, 0xD6, 
    0x3F, 0x2D, 0x80, 0xFE, 0x36, 0x41, 0x1B, 0x20, 0xF1, 0xE4, 0xD3, 0xEF, 0x02, 0x48, 0x6D, 0x61, 
    0x69, 0x97, 0xE2, 0x83, 0xDD, 0x06, 0x00, 0x68, 0x95, 0xF4, 0x6F, 0x00, 0x2C, 0xD0, 0x08, 0x63, 
    0x0A, 0xF0, 0x40, 0x02, 0xA0, 0xBE, 0xCC, 0xDE, 0x54, 0x03, 0x30, 0x0C, 0x4D, 0xF0, 0x5D, 0x04, 
    0x00, 0x20, 0x1E, 0x06, 0x57, 0x80, 0x8C, 0x81, 0x64, 0x20, 0x03, 0x64, 0xC5, 0x2B, 0x94, 0xBD, 
    0xBC, 0xDB, 0x00, 0x3C, 0x0C, 0x5F, 0x0B, 0xA0, 0x10, 0xB0, 0x40, 0x52, 0x09, 0x03, 0x91, 0x1E, 
    0xC6, 0x74, 0x05, 0x00, 0x80, 0x13, 0x88, 0xFF, 0x88, 0x07, 0xC1, 0x19, 0xE0, 0x94, 0x0B, 0xD7, 
    0xC5, 0x00, 0x34, 0x81, 0x5C, 0x9B, 0x94, 0x9C, 0xBD, 0x23, 0x94, 0x2F, 0x00, 0xFC, 0x8D, 0x17, 
    0x72, 0x95, 0xD5, 0xFA, 0x20, 0x12, 0x00, 0xF8, 0x27, 0x00, 0xA9, 0xE8, 0xD5, 0xFA, 0x31, 0xC1, 
    0xC8, 0x05, 0x2A, 0xCF, 0x47, 0x42, 0x68, 0x04, 0x9A, 0x8E, 0x90, 0x0D, 0xC0, 0x9B, 0x5E, 0x2D, 
    0x01, 0x70, 0x46, 0x03, 0xA0, 0x57, 0xEF, 0x06, 0x40, 0xDE, 0x98, 0x60, 0x07, 0x48, 0xEF, 0xCF, 
    0x90, 0xBB, 0xD5, 0xDE, 0x4F, 0x6B, 0x4B, 0xC9, 0x68, 0xA7, 0xFB, 0x32, 0x00, 0x04, 0xCC, 0x86, 
    0xF8, 0xA0, 0x23, 0xF9, 0x5D, 0x7F, 0x14, 0x40, 0xDD, 0x66, 0x64, 0xC3, 0xEC, 0x16, 0xC5, 0x91, 
    0x87, 0xF4, 0x8E, 0x23, 0x77, 0xC6, 0x4D, 0xAA, 0x62, 0xD6, 0xA5, 0x00, 0x40, 0x40, 0x61, 0x60, 
    0x80, 0xAD, 0x8F, 0xB1, 0x47, 0xA1, 0x1D, 0xD7, 0xD3, 0x51, 0x95, 0xF7, 0x25, 0x27, 0x00, 0x55, 
    0xE3, 0xAC, 0x7E, 0xCF, 0x9B, 0x5C, 0xD9, 0x11, 0xBE, 0x02, 0x6C, 0x9B, 0xA8, 0x6B, 0x3E, 0x2C, 
    0x08, 0x09, 0x06, 0x98, 0x11, 0x38, 0x9A, 0x3F, 0x88, 0x25, 0x75, 0xE2, 0x67, 0xA8, 0x70, 0x1E, 
    0xAC, 0xF3, 0x68, 0xDA, 0x75, 0x5B, 0x00, 0x40, 0x26, 0x41, 0x4A, 0x82, 0xA4, 0xF2, 0x4C, 0x1F, 
    0x11, 0x7C, 0xE0, 0x74, 0x92, 0x8E, 0x49, 0x0C, 0x9E, 0xAE, 0x66, 0x68, 0xDD, 0xAE, 0x8B, 0x05, 
    0xAE, 0x27, 0xF9, 0x5F, 0x29, 0xCA, 0xE6, 0x64, 0x86, 0xFF, 0xAD, 0x32, 0x26, 0x88, 0x7C, 0xED, 
    0x5D, 0xAC, 0x3E, 0x10, 0x8E, 0xA8, 0x4F, 0x51, 0xA9, 0x70, 0xC7, 0x2F, 0x01, 0x20, 0x04, 0x18, 
    0x81, 0x48, 0x8A, 0x05, 0x26, 0xC0, 0xF6, 0x4F, 0x32, 0x62, 0xCC, 0x8F, 0xF8, 0x67, 0x01, 0x38, 
    0x00, 0x04, 0xC0, 0x04, 0x00, 0xE0, 0x03, 0x09, 0x2E, 0xB8, 0x9A, 0x80, 0xF9, 0x34, 0xEA, 0xF5, 
    0xEF, 0xB3, 0x41, 0xA8, 0x7F, 0x19, 0x20, 0x7C, 0x00, 0x64, 0x10, 0x00, 0xC8, 0x9C, 0x05, 0x00, 
    0xAD, 0x53, 0x31, 0x81, 0x75, 0x8E, 0x26, 0xAC, 0xF9, 0x19, 0x7D, 0xE1, 0xCB, 0x23, 0xFA, 0x3E, 
    0x00, 0x48, 0x00, 0x38, 0x0A, 0x40, 0xE9, 0x88, 0xB6, 0x76, 0x0A, 0x92, 0x31, 0x8B, 0xA3, 0x64, 
    0xBA, 0x80, 0xC4, 0x3E, 0xE5, 0xF2, 0xD2, 0x0E, 0x16, 0xF2, 0xA1, 0x4B, 0x10, 0x3C, 0x46, 0xCB, 
    0x00, 0x4C, 0xE9, 0x59, 0x19, 0xCD, 0xC1, 0x8B, 0x8D, 0xE1, 0x00, 0xC4, 0xC5, 0xEA, 0xF9, 0xCE, 
    0x7C, 0xCA, 0x71, 0xB1, 0x23, 0xFF, 0xF5, 0x00, 0x60, 0x34, 0x95, 0xBF, 0x9A, 0x00, 0x00, 0xD7, 
    0x14, 0x06, 0x98, 0xC7, 0x57, 0xE9, 0x9F, 0x78, 0x98, 0xE2, 0xF3, 0x7D, 0x49, 0x48, 0x20, 0x07, 
    0x21, 0x98, 0xC2, 0x76, 0x1E, 0x00, 0x80, 0x92, 0x08, 0x80, 0x9F, 0x6C, 0xDB, 0xA2, 0xB6, 0x64, 
    0xDE, 0x70, 0x5B, 0x29, 0x15, 0x0A, 0xEB, 0x97, 0xF5, 0x37, 0x95, 0x6E, 0x17, 0x04, 0x77, 0x58, 
    0xB9, 0x24, 0x41, 0x7F, 0x00, 0x32, 0x1F, 0xE7, 0x0D, 0x3D, 0x52, 0x1B, 0x49, 0xB9, 0xFE, 0x08, 
    0x80, 0xF4, 0xE6, 0xB3, 0x00, 0x30, 0x86, 0x01, 0xBE, 0x76, 0x00, 0x0E, 0x74, 0x5D, 0x9A, 0x53, 
    0x58, 0x37, 0x82, 0x7A, 0xEA, 0x43, 0x40, 0x47, 0x3D, 0xA9, 0xCD, 0x6B, 0xDB, 0xDB, 0x4E, 0x90, 
    0x2F, 0xFD, 0xBB, 0x00, 0x70, 0x4D, 0x9A, 0xF9, 0x12, 0x05, 0x90, 0x51, 0x15, 0xA2, 0xBF, 0x26, 
    0xC2, 0x2E, 0xD0, 0xA9, 0xF0, 0x4B, 0x00, 0x20, 0x8C, 0x15, 0xB9, 0x10, 0x58, 0xFF, 0x04, 0x60, 
    0x1F, 0xA1, 0x3E, 0x54, 0xBD, 0x0A, 0x20, 0xC3, 0xD0, 0x1D, 0xDD, 0x4C, 0x46, 0x02, 0xA8, 0x3F, 
    0xBB, 0x26, 0x2A, 0x26, 0xA5, 0xE5, 0xE3, 0x94, 0x0E, 0x70, 0xC2, 0x0F, 0xF4, 0x07, 0x80, 0x97, 
    0xE0, 0x04, 0x13, 0x00, 0xF4, 0x8F, 0xEA, 0xDE, 0x89, 0x50, 0x00, 0x1F, 0xC5, 0x1F, 0x84, 0xF7, 
    0x02, 0xF8, 0xCD, 0x62, 0x8B, 0xF5, 0xAF, 0x77, 0xCB, 0xCC, 0xC0, 0xDB, 0x32, 0x28, 0xCD, 0xF6, 
    0x4D, 0xB9, 0xDF, 0x58, 0x01, 0x48, 0x39, 0x81, 0x48, 0xBF, 0x5F, 0x2B, 0xE8, 0xA2, 0xB0, 0x10, 
    0xFA, 0xA3, 0x35, 0xB3, 0xD5, 0xA0, 0x41, 0x20, 0x6C, 0x42, 0xB0, 0x01, 0x50, 0xE4, 0xCD, 0x2B, 
    0x00, 0x62, 0x18, 0x5C, 0xAD, 0x93, 0xBC, 0xBA, 0x12, 0x2F, 0xA4, 0x24, 0xEB, 0xAA, 0x4A, 0x25, 
    0xD9, 0x02, 0xF8, 0x43, 0x30, 0xBC, 0x5E, 0x03, 0x00, 0x82, 0xE4, 0xDF, 0xC7, 0x00, 0x58, 0xB2, 
    0x0C, 0x1F, 0x82, 0xA5, 0x28, 0xB3, 0x01, 0x00, 0xA7, 0x9A, 0xF4, 0xA9, 0xAA, 0xB8, 0xF5, 0x7F, 
    0x97, 0x13, 0x28, 0xD2, 0x8A, 0x20, 0x2E, 0x98, 0xDD, 0xB8, 0x02, 0x98, 0xCD, 0x53, 0x3B, 0x41, 
    0x65, 0x29, 0x14, 0x65, 0xA5, 0x8B, 0xC0, 0x75, 0xB0, 0x3C, 0x2E, 0xFD, 0xDF, 0x5D, 0x17, 0x37, 
    0x47, 0x47, 0xE3, 0x7B, 0x00, 0xDC, 0xAE, 0x00, 0x51, 0x1F, 0x1B, 0xB0, 0x93, 0x21, 0x02, 0xBF, 
    0x43, 0x74, 0xA4, 0x4C, 0xF4, 0x2C, 0x81, 0x01, 0xB4, 0x2C, 0x69, 0x00, 0x08, 0x3A, 0x1C, 0xBB, 
    0x40, 0x86, 0x20, 0x00, 0x8A, 0x46, 0x59, 0x20, 0x62, 0x0B, 0xAC, 0x48, 0x07, 0x80, 0x91, 0xA6, 
    0x1A, 0x47, 0x56, 0x48, 0x23, 0x71, 0x1B, 0x1B, 0xFD, 0x16, 0xD6, 0x8A, 0x7A, 0xCF, 0x26, 0x00, 
    0xED, 0x45, 0x22, 0xF8, 0x2F, 0xD6, 0x1A, 0x0E, 0x23, 0x2F, 0x3D, 0x86, 0x10, 0x00, 0x00, 0x00, 
    0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82
};

struct DemoWorld
{
    gfx3d::Lights *lights;
    raylib::Model *terrain;
    phys3d::World *physics;

    DemoWorld(const core::App* app, const gfx3d::Camera* camera, const Vector3& size)
    {
        Image heightmap = LoadImageFromMemory(".png", map, sizeof(map));

        Mesh meshHeightmap = GenMeshHeightmap(heightmap, size);
        GenMeshTangents(&meshHeightmap); // Generation of tangents for calculating normals with the normalMap (grass_normal)

        terrain = new raylib::Model(meshHeightmap);
        UnloadImage(heightmap);

        terrain->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture(TextFormat("%s/../../../examples/resources/images/grass_diffuse.png", GetApplicationDirectory()));
        terrain->materials[0].maps[MATERIAL_MAP_SPECULAR].texture = LoadTexture(TextFormat("%s/../../../examples/resources/images/grass_specular.png", GetApplicationDirectory()));
        terrain->materials[0].maps[MATERIAL_MAP_NORMAL].texture = LoadTexture(TextFormat("%s/../../../examples/resources/images/grass_normal.png", GetApplicationDirectory()));
        physics = new phys3d::World({ 0, -9.81, 0 });

        auto w = physics->AddObject<phys3d::Model>(
            Vector3{ -size.x * 0.5f, 0, -size.z * 0.5f },
            Vector3{ 0, 0, 0 },
            terrain);

        lights = new gfx3d::Lights(DARKGRAY, 1, 1024);
        lights->AddShadowLight(gfx3d::Camera({ -size.x * 0.5f, 32, -size.z * 0.5f }, {}));
    }

    ~DemoWorld()
    {
        delete lights;
        delete terrain;
        delete physics;
    }

    void Update(gfx3d::Camera* camera)
    {
        physics->Step(1.0f/60.0f, 10);
        lights->Update(*camera);

        for (auto& object : *physics)
        {
            lights->DrawDepth(*object->model, object->GetPosition(), object->GetRotationAxis(), object->GetAngle());
        }
    }

    void Draw()
    {
        for (auto& object : *physics)
        {
            lights->Draw(*object->model, object->GetPosition(), object->GetRotationAxis(), object->GetAngle(), { 1, 1, 1 }, object->color);
        }
    }
};

class Demo : public core::State
{
  private:
    gfx3d::Camera *camera;
    DemoWorld *world;

  public:
    void Enter() override
    {
        camera = new gfx3d::Camera(
            { 0.0f, 16.0f, -32.0f },
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            60.0f, app->GetAspectRatio());

        world = new DemoWorld(app, camera, { 64, 8, 64 });

        core::RandomGenerator gen;

        for (int i = 0; i < 10; i++)
        {
            if (gen.Random<uint8_t>(0, 1))
            {
                world->physics->AddObject<phys3d::Cube>(
                    gen.RandomVec3({ -5, 10, -5 }, { 5, 15, 5 }),
                    gen.RandomVec3({ -3, -3, -3 }, { 3, 3, 3 }),
                    gen.RandomVec3({ 1, 1, 1 }, { 3, 3, 3 }),
                    1.0f, gen.RandomColor()
                )->model->materials[0].maps[MATERIAL_MAP_SPECULAR].texture = {
                    rlGetTextureIdDefault(), 1, 1, 1
                };
            }
            else
            {
                world->physics->AddObject<phys3d::Sphere>(
                    gen.RandomVec3({ -5, 10, -5 }, { 5, 15, 5 }),
                    gen.RandomVec3({ -3, -3, -3 }, { 3, 3, 3 }),
                    gen.Random<float>(1, 3), 64, 64, 1.0f, gen.RandomColor()
                )->model->materials[0].maps[MATERIAL_MAP_SPECULAR].texture = {
                    rlGetTextureIdDefault(), 1, 1, 1
                };
            }
        }

        DisableCursor();
    }

    void Exit() override
    {
        delete world;
        delete camera;
        EnableCursor();
    }

    void Update(float dt) override
    {
        camera->Update(CAMERA_FIRST_PERSON);
        world->Update(camera);
    }

    void Draw(const core::Renderer& target) override
    {
        target.Clear();

        camera->BeginMode();
            world->Draw();
        camera->EndMode();
    }
};

int main()
{
    core::App app("PHYS 3D - World", 800, 600, FLAG_MSAA_4X_HINT);
    app.AddState<Demo>("demo");
    return app.Run("demo");
}
