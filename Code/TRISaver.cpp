#include <windows.h>
#include <scrnsave.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <ctime>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINAPI RegisterDialogClasses(HANDLE hInst);
void Render();

HDC hDC;
HGLRC hRC;

double x_angl = 0.0f;
double y_angl = 0.0f;
double z_angl = 0.0f;

double target_x;
double target_y;
double target_z;

bool final_target;

const float final_target_x = 63.0;
const float final_target_y = 178.5;
const float final_target_z = 49.5;

const int TARGET_TIMER = 0;
const int UPDATE_TIMER = 1;

UINT_PTR target_timer_id;
UINT_PTR update_timer_id;

LRESULT CALLBACK ScreenSaverProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,  
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	int pfid;

	GLint glnWidth;
	GLint glnHeight;

	switch(msg)
	{
		case WM_CREATE:
			hDC = GetDC(hWnd);

			pfid = ChoosePixelFormat(hDC, &pfd);
			SetPixelFormat(hDC, pfid, &pfd);
			
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);
			
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glEnable(GL_COLOR_MATERIAL);
			
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1.0f);

			target_timer_id = SetTimer(hWnd, TARGET_TIMER, 10000, NULL);
			update_timer_id = SetTimer(hWnd, UPDATE_TIMER, 1, NULL);

			srand((unsigned)time(NULL));
			target_x = rand() % 360;
			target_y = rand() % 360;
			target_z = rand() % 360;
			return 0;
		
		case WM_DESTROY:
			KillTimer(hWnd, target_timer_id);
			KillTimer(hWnd, update_timer_id);

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hRC);
			ReleaseDC(hWnd, hDC);

			PostQuitMessage(0);
			return 0;

		case WM_SIZE:
			glnWidth  = (GLsizei) LOWORD (lParam);
			glnHeight = (GLsizei) HIWORD (lParam);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(20.0, (GLdouble)glnWidth/(GLdouble)glnHeight, 1.0f, 512.0f);

			glViewport(0, 0, glnWidth, glnHeight);
			return 0;

		case WM_TIMER:
			switch(wParam)
			{
				case TARGET_TIMER:
					final_target = !final_target;
					if (final_target)
					{
						target_x = final_target_x;
						target_y = final_target_y;
						target_z = final_target_z;
					}
					else
					{
						target_x = rand() % 360;
						target_y = rand() % 360;
						target_z = rand() % 360;
					}
					break;

				case UPDATE_TIMER:
					x_angl += 0.175f * (target_x - x_angl);
					y_angl += 0.175f * (target_y - y_angl);
					z_angl += 0.175f * (target_z - z_angl);

					Render();
					SwapBuffers(hDC);
					break;

				default:
					break;
			}
			return 0;

		default:
			break;
	}
	
	return DefScreenSaverProc(hWnd, msg, wParam, lParam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_INITDIALOG:
			MessageBox(NULL, "No Configuration Options Available", "TRI", MB_OK);
			DestroyWindow(hDlg);
			return TRUE;
		default:
			break;
	}

	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
	return TRUE;
}

void Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -50.0f);

	GLfloat diff[] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat spec[] = {0.0f, 0.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

	GLfloat dir[] = {-0.577f, -0.577f, -0.577f};
	GLfloat pos[] = {5.0f, 5.0f, 5.0f, 0.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, pos);

	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 2.0f);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 1.0f);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, dir);
	
	glColor3f(0.0f, 0.0f, 1.0f);
	GLfloat mat_diff[] = {0.0f, 0.0f, 1.0f, 1.0f};
	GLfloat mat_spec[] = {0.0f, 0.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_spec);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

	glRotated(x_angl, 1.0, 0.0, 0.0);
	glRotated(y_angl, 0.0, 1.0, 0.0);
	glRotated(z_angl, 0.0, 0.0, 1.0);
	
	glTranslatef(0.0f, 2.0f, 0.0f);

	glBegin(GL_QUADS);

		/* Middle section
		   Length: 10.0cm
		   Width: 1.0cm */

	    // Top
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-5.0f, -4.5f, 0.5f);
		glVertex3f(5.0f, -4.5f, 0.5f);
		glVertex3f(5.0f, -4.5f, -0.5f);
		glVertex3f(-5.0f, -4.5f, -0.5f);

		// Front
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-5.0f, -4.5f, 0.5f);
		glVertex3f(-5.0f, -5.5f, 0.5f);
		glVertex3f(5.0f, -5.5f, 0.5f);
		glVertex3f(5.0f, -4.5f, 0.5f);

		// Bottom
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-5.0f, -5.5f, 0.5f);
		glVertex3f(6.0f, -5.5f, 0.5f);
		glVertex3f(6.0f, -5.5f, -0.5f);
		glVertex3f(-5.0f, -5.5f, -0.5f);

		// Back
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(-6.0f, -4.5f, -0.5f);
		glVertex3f(5.0f, -4.5f, -0.5f);
		glVertex3f(5.0f, -5.5f, -0.5f);
		glVertex3f(-6.0f, -5.5f, -0.5f);

		/* Left section
		   Length: 9.0cm
		   Width: 1.0cm */

		// Top
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-5.0f, -4.5f, -0.5f);
		glVertex3f(-5.0f, -4.5f, 8.5f);
		glVertex3f(-6.0f, -4.5f, 8.5f);
		glVertex3f(-6.0f, -4.5f, -0.5f);

		// Left
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-6.0f, -4.5f, -0.5f);
		glVertex3f(-6.0f, -4.5f, 8.5f);
		glVertex3f(-6.0f, -5.5f, 8.5f);
		glVertex3f(-6.0f, -5.5f, -0.5f);

		// Bottom
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-6.0f, -5.5f, -0.5f);
		glVertex3f(-6.0f, -5.5f, 8.5f);
		glVertex3f(-5.0f, -5.5f, 8.5f);
		glVertex3f(-5.0f, -5.5f, -0.5f);

		// Right
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-5.0f, -4.5f, -0.5f);
		glVertex3f(-5.0f, -5.5f, -0.5f);
		glVertex3f(-5.0f, -5.5f, 8.5f);
		glVertex3f(-5.0f, -4.5f, 8.5f);

		// End
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-6.0f, -4.5f, 8.5f);
		glVertex3f(-6.0f, -5.5f, 8.5f);
		glVertex3f(-5.0f, -5.5f, 8.5f);
		glVertex3f(-5.0f, -4.5f, 8.5f);
		
		/* Right section
		   Length: 9.0cm
		   Width: 1.0cm */

		// Front
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(5.0f, -5.5f, 0.5f);
		glVertex3f(5.0f, 4.5f, 0.5f);
		glVertex3f(6.0f, 4.5f, 0.5f);
		glVertex3f(6.0f, -5.5f, 0.5f);

		// Left
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(5.0f, -5.5f, 0.5f);
		glVertex3f(5.0f, -5.5f, -0.5f);
		glVertex3f(5.0f, 2.3f, -0.5f);
		glVertex3f(5.0f, 2.3f, 0.5f);

		// Back
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(5.0f, -5.5f, -0.5f);
		glVertex3f(6.0f, -5.5f, -0.5f);
		glVertex3f(6.0f, 3.0f, -0.5f);
		glVertex3f(5.0f, 3.0f, -0.5f);

		// Right
		glNormal3f(1.0f, 0.0f, 0.0f);
		glVertex3f(6.0f, -5.5f, 0.5f);
		glVertex3f(6.0f, -5.5f, -0.5f);
		glVertex3f(6.0f, 4.5f, -0.5f);
		glVertex3f(6.0f, 4.5f, 0.5f);

		// Top
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(5.0f, 4.5f, -0.5f);
		glVertex3f(5.0f, 4.5f, 0.5f);
		glVertex3f(6.0f, 4.5f, 0.5f);
		glVertex3f(6.0f, 4.5f, -0.5f);
	glEnd();

	glBegin(GL_TRIANGLES);
		// Left
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(5.0f, 3.5f, 0.5f);
		glVertex3f(5.0f, 4.5f, -0.5f);
		glVertex3f(5.0f, 4.5f, 0.5f);
	
		// Back
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(5.0f, 3.0f, -0.5f);
		glVertex3f(6.0f, 3.0f, -0.5f);
		glVertex3f(6.0f, 3.9f, -0.5f);
	glEnd();

	glFlush();
}