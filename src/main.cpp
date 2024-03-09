#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <sstream>
#include <cassert>


#include "handmade_math.h"

struct win32_offscreen_buffer
{
	int Width;
	int Height;
	int BytesPerPixel;
	int Pitch;
	void* Memory;
	BITMAPINFO Info;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};
#define global_variable static
#define internal static

global_variable win32_offscreen_buffer BackBuffer;

#define BUFFER_WIDTH 2000
#define BUFFER_HEIGHT 1000
#define PUT_PIXEL(x,y,color)  {\
								if ((x)> -BUFFER_WIDTH/2 && (x) < BUFFER_WIDTH/2 && (y)> -BUFFER_HEIGHT/2 && (y) < BUFFER_HEIGHT/2) \
								{ *((u32*)BackBuffer.Memory+(y)*BUFFER_WIDTH+(x)) = (color); } \
							  }

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
internal void Win32ResizeDIBSection(win32_offscreen_buffer* Buffer, int Width, int Height);
v3 O = {0,0,0};
struct loaded_bitmap
{
    v2  AlignPercentage;
    r32 WidthOverHeight;

    void *Memory;
    s32   Width;
    s32   Height;
    s32   Pitch;
};
internal loaded_bitmap LoadBMP(const char *Filename);
#include "raytrace.cpp"
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        return 0;
    }
    HDC DeviceContext = GetDC(hwnd);

    //---[MAGIC]-------------------------------------------------------
    Win32ResizeDIBSection(&BackBuffer, BUFFER_WIDTH, BUFFER_HEIGHT);
    void *VideoMemory = BackBuffer.Memory;
    BackBuffer.Memory = (void*)((u8*)BackBuffer.Memory + BackBuffer.Pitch * (BUFFER_HEIGHT/2) + BUFFER_WIDTH/2*BackBuffer.BytesPerPixel);
    RayTracer();
    //---[MAGIC]-------------------------------------------------------
    
    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        StretchDIBits(DeviceContext,
			0, 0, BUFFER_WIDTH, BUFFER_HEIGHT,
			0, 0, BUFFER_WIDTH, BUFFER_HEIGHT,
			VideoMemory,
			&BackBuffer.Info,
			DIB_RGB_COLORS, SRCCOPY);
    
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
        
    case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
    {
		const int Press = (HIWORD(lParam) & KF_UP) ? 0 : 1;
		u32 VKCode = (u32)wParam;
		if (Press)
		{
			switch (VKCode)
			{
				case 'W':       ; break;
				case 'S':       ; break;
				case 'A':       ; break;
				case 'D':       ; break;
				case VK_UP:     O.y+=0.3; RayTracer(); break;
				case VK_DOWN:   O.y-=0.3; RayTracer(); break;
				case VK_LEFT:   O.x-=0.3; RayTracer(); break;
				case VK_RIGHT:  O.x+=0.3; RayTracer(); break;
				case 'X':       ; break;
			}
		}
		else
		{
			switch (VKCode)
			{
				case 'W':      ; break;
				case 'S':      ; break;
				case 'A':      ; break;
				case 'D':      ; break;
				case VK_UP:    ; break;
				case VK_DOWN:  ; break;
				case VK_LEFT:  ; break;
				case VK_RIGHT: ; break;
				case 'X':      ; break;
			}
		}
		
		break;
    }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


internal void
Win32ResizeDIBSection(win32_offscreen_buffer* Buffer, int Width, int Height)
{

	if (Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4;
	Buffer->Pitch = Buffer->BytesPerPixel * Width;
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = Buffer->Height;  // If biHeight is positive, the bitmap is a bottom - up DIB with the origin at the lower left corner.
	Buffer->Info.bmiHeader.biPlanes = 1;               // If biHeight is negative (-Buffer->Height), the bitmap is a top - down DIB with the origin at the upper left corner.
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Width * Height) * Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    for (int i = 0; i < (Width * Height); i++) 
        ((u32*)Buffer->Memory)[i] = 0xFF666666;
}




#pragma pack(push, 1)
struct bitmap_header
{
    u16 FileType;
    u32 FileSize;
    u16 FileReserved1;
    u16 FileReserved2;
    u32 FileOffBits;

    u32 Size;
    u32 Width;
    u32 Height;
    u16 Planes;
    u16 BitCount;

    u32 Compression;
    u32 SizeImage;
    s32 XPelsPerMeter;
    s32 YPelsPerMeter;
    u32 ClrUsed;
    u32 ClrImportant;
    u32 RedMask;
    u32 GreenMask;
    u32 BlueMask;
    u32 AlphaMask;
};
#pragma pack(pop)

#define BYTES_PER_PIXEL 4

struct bit_scan_result
{
    b32 Found;
    u32 Index;
};
inline bit_scan_result
FindLeastSignificantSetBit(u32 Value)
{
    bit_scan_result Result = {};
    Result.Found = _BitScanForward((unsigned long *) &Result.Index, Value);
	return (Result);
}

struct read_file_result
{
    void *Contents;
    u32   ContentsSize;
};

read_file_result 
ReadFile(const char* Filename)
{
    read_file_result Result = {};

    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ,
                                    0, OPEN_EXISTING, 0, 0);

    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(FileHandle, &FileSize))
        {
			assert(FileSize.QuadPart <= 0xFFFFFFFF);
			u32 FileSize32 = (u32) FileSize.QuadPart;
            Result.Contents = VirtualAlloc(0, FileSize32,
                                           MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            if (Result.Contents)
            {
                DWORD BytesRead;
                if (ReadFile(FileHandle, Result.Contents, FileSize32, &BytesRead, 0) && (FileSize32 == BytesRead))
                {
                    Result.ContentsSize = FileSize32;
                } else
                {
                    VirtualFree(Result.Contents, 0, MEM_RELEASE);
                    Result.Contents = 0;
                }
            } else
            {
                //TODO(rahul): Logging
            }
        } else
        {
            //TODO(rahul): Logging
        }

        CloseHandle(FileHandle);
    } else
    {
        //TODO(rahul): Logging
    }
    return (Result);
}

internal loaded_bitmap
LoadBMP(const char *Filename)
{
    read_file_result ReadResult = ReadFile(Filename);

    loaded_bitmap Result = {};
	if (ReadResult.ContentsSize != 0)
	{
		bitmap_header* Header = (bitmap_header*)ReadResult.Contents;
		u32* Pixels = (u32*)((u8*)ReadResult.Contents + Header->FileOffBits);

		Result.Width = Header->Width;
		Result.Height = Header->Height;
		Result.Memory = Pixels;

		Result.WidthOverHeight = SafeRatio0((r32)Result.Width, (r32)Result.Height);
		Result.Pitch = Result.Width * BYTES_PER_PIXEL;

#if 0
		Result.Memory = (u8*)Result.Memory + Result.Pitch * (Result.Height - 1);
		Result.Pitch = -Result.Pitch;
#endif
		//u32* source = Pixels;
		//for (u32 Y = 0; Y < Header->Height; ++Y) {
		//	for (u32 X = 0; X < Header->Width; ++X) {
		//		*(u32*)((u8*)VideoMemory + (Y * BUFFER_WIDTH * 4) + (X * 4)) = *source++;
		//	}
		//}
	}
    return (Result);
}