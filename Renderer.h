// Renderer.h
#pragma once

#ifndef RENDERER_H
#define RENDERER_H

#include "framework.h"
#include <gl/GL.h>
#include <GL/glu.h>
#include "Window.h"

// ������� ��� ������ � OpenGL
void LoadOpenGLFunctions();
void InitOpenGL(HWND hWnd);
void CleanupOpenGL(HWND hWnd);
void DrawGrid();


#endif //RENDERER_H