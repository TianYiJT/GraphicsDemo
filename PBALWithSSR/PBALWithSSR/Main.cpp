
#include"glConfigruation.h"
#include"SSREffect.h"

SceneManager *mSceneManager = new SceneManager();
SSREffect *mSSREffect = new SSREffect();
PBALRenderer *mPBALRenderer = new PBALRenderer();
int lastX = 0, lastY = 0;
bool rotating=false, scaling=false;
void Update() 
{
	glEnable(GL_MULTISAMPLE);
	mSSREffect->preprocess(mSceneManager,mPBALRenderer);
	mSSREffect->Render(mSceneManager);
	//std::cout << "update" << std::endl;
}

void Init() 
{
	mSceneManager->InitGameObject();
	mSceneManager->InitLight();
	mSceneManager->InitVAO();
	mSSREffect->Init(mSceneManager);
	mPBALRenderer->Init();
}
void OnMouseMove(int x,int y) 
{
	int mx = x - lastX;
	int my = y - lastY;
	lastX = x;
	lastY = y;
	if (rotating)
	{
		mSceneManager->camera.rotation(mx, my,0);
	}
	if (scaling)
	{
		mSceneManager->camera.move(mx);
	}
}
void OnMouse(int button, int state, int x, int y)
{


	if (button == GLUT_LEFT_BUTTON&&state == GLUT_DOWN) 
	{
		rotating = true;
	}
	if (button == GLUT_LEFT_BUTTON&&state == GLUT_UP)
	{
		rotating = false;
	}
	if (button == GLUT_MIDDLE_BUTTON&&state == GLUT_DOWN)
	{
		scaling = true;
	}
	if (button == GLUT_MIDDLE_BUTTON&&state == GLUT_UP)
	{
		scaling = false;
	}
}

void processSpecialKeys(int key, int x, int y) {

	switch (key)
	{
	case GLUT_KEY_UP:
		mSceneManager->camera.translation((float)0 / (float)800.0, (float)50 / (float)600.0);
		break;
	case GLUT_KEY_DOWN:
		mSceneManager->camera.translation((float)0 / (float)800.0, -(float)50 / (float)600.0);
		break;
	case GLUT_KEY_LEFT:
		mSceneManager->camera.translation(-(float)50 / (float)800.0, (float)0 / (float)600.0);
		break;
	case GLUT_KEY_RIGHT:
		mSceneManager->camera.translation((float)50 / (float)800.0, (float)0 / (float)600.0);
		break;
	}
}
int main(int argc,char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutCreateWindow("PBAL With SSR Demo");
	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	if (glewInit())
	{
		exit(EXIT_FAILURE);
	}
	Init();
	glutDisplayFunc(Update);
	//glutMouseFunc(&OnMouse);
//	glutSpecialFunc(processSpecialKeys);
	//glutMotionFunc(OnMouseMove);
	glutMainLoop();
	return 0;
}