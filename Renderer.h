// Renderer.h
#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include "framework.h"
#include <gl/GL.h>
#include "Window.h"

// ������� ��� ������ � OpenGL
void InitOpenGL(HWND hWnd);
void CleanupOpenGL(HWND hWnd);
void DrawGrid();

#endif //RENDERER_H