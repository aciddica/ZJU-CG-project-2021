#include "main.h"

void key(unsigned char k, int x, int y) {
    cout << k << endl;
    switch (k) {
        case ' ':{ ifmainlight = !ifmainlight; break; } // Main light Switch
        case '0':{ operation = 0; break; } // Main light Pos
        case '1':{ operation = 1; break; } // Main light Color
        case '4':{ ifflashlight = !ifflashlight; break; } // Flashlight Switch
        case '5':{ ifspotlight = !ifspotlight; break; } // Spotlight Switch
        case 27:
        case 'q': {exit(0); break; }
        case 'l': {
            if (operation == 0){
                MainLightPos[0] += MainLightPosVar;
            }
            else if (operation == 1){
                if (MainLightColor[0] + MainLightColorVar <= 1.0)
                    MainLightColor[0] += MainLightColorVar;
            }
            break;
        }
        
        case 'j': {
            if (operation == 0){
                MainLightPos[0] -= MainLightPosVar;
            }
            else if (operation == 1){
                if (MainLightColor[0] - MainLightColorVar >= 0.0)
                    MainLightColor[0] -= MainLightColorVar;
            }
            break;
        }
        case 'k':
        {
            if (operation == 0){
                MainLightPos[1] -= MainLightPosVar;
            }
            else if (operation == 1){
                if (MainLightColor[1] - MainLightColorVar >= 0.0)
                    MainLightColor[1] -= MainLightColorVar;
            }
            break;
        }
        case 'i':{
            if (operation == 0){
                MainLightPos[1] += MainLightPosVar;
            }
            else if (operation == 1){
                if (MainLightColor[1] + MainLightColorVar <= 1.0)
                    MainLightColor[1] += MainLightColorVar;
            }
            break;
        }
        case 'c':
        {
            if (operation == 0){
                MainLightPos[2] -= MainLightPosVar;
            }
            else if (operation == 1){
                if (MainLightColor[2] - MainLightColorVar >= 0.0)
                    MainLightColor[2] -= MainLightColorVar;
            }
            break;
        }
        case 'v':
        {
            if (operation == 0){
                MainLightPos[2] += MainLightPosVar;
            }
            else if (operation == 1){
                if (MainLightColor[2] + MainLightColorVar <= 1.0)
                    MainLightColor[2] += MainLightColorVar;
            }
            break;
        }
        case 't': {
            SPlightDir[2] -= 0.05;
            break;
        }
        case 'f': {
            SPlightDir[0] -= 0.05;
            break;
        }
        case 'g': {
            SPlightDir[2] += 0.05;
            break;
        }
        case 'h': {
            SPlightDir[0] += 0.05;
            break;
        }
        case 'n': {
            SPspotangle -= 1.0f;
            if( SPspotangle < 0.0f ) SPspotangle = 30.0f;
            break;
        }
        case 'm': {
            SPspotangle += 1.0f;
            if( SPspotangle > 60.0 ) SPspotangle = 30.0f;
            break;
        }
        case 'b': {
            light_color_index++;
            if(light_color_index > 9) light_color_index = 0;
            break;
        }
    }
}

void display() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 1, 0);
    
    //大灯
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, MainLightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, MainLightColor);
    if( ifmainlight )
    {
        glEnable(GL_LIGHT0);
    }
    else
    {
        glDisable(GL_LIGHT0);
    }

    //手电筒
    glLightfv(GL_LIGHT1, GL_AMBIENT, FLambient);          //设置环境光成分
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);            //设置漫射光成分
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);           //设置镜面光成分
    glLightfv(GL_LIGHT1, GL_POSITION, FLposition);        //设置光源位置
    glLightf(GL_LIGHT1,  GL_SPOT_CUTOFF, FLspotangle);    //裁减角度
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, FLlightDir);  //光源方向
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 30);
    if( ifflashlight )
    {
        glEnable(GL_LIGHT1);
    }
    else
    {
        glDisable(GL_LIGHT1);
    }
    
    //聚光灯
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_color[light_color_index]);          //设置环境光成分
    glLightfv(GL_LIGHT2, GL_DIFFUSE, white);            //设置漫射光成分
    glLightfv(GL_LIGHT2, GL_SPECULAR, white);           //设置镜面光成分
    glLightfv(GL_LIGHT2, GL_POSITION, SPposition);        //设置光源位置
    glLightf(GL_LIGHT2,  GL_SPOT_CUTOFF, SPspotangle);    //裁减角度
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, SPlightDir);  //光源方向
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 30);
    if( ifspotlight )
    {
        glEnable(GL_LIGHT2);
    }
    else
    {
        glDisable(GL_LIGHT2);
    }
    
    glPushMatrix();
    glTranslatef(0, 0, 4+1);
    glRotatef(90, 1, 0, 0);
    glutSolidTeapot(1);
    glPopMatrix();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75.0f, (float)w / h, 1.0f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

void init()
{
    // 启用点平滑处理
    glEnable(GL_POINT_SMOOTH);
    // 设置为画质最优的方式
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    // 启用直线平滑处理
    glEnable(GL_LINE_SMOOTH);
    // 设置为画质最优的方式
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // 启用多边形平滑处理
    glEnable(GL_POLYGON_SMOOTH);
    // 设置为画质最优的方式
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void idle()
{
    glutPostRedisplay();
}

int main(int argc, char *argv[]) {
    cout << "ok" << endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("CG Project - Ours");
    
    init();
    
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(key);
    glutMainLoop();
    
    return 0;
}
