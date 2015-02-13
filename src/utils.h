#pragma once

#define SAFE_DELETE(p) if (p != nullptr) { delete p; p = nullptr; }

