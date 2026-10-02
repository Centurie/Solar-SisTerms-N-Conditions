#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <array>
#include <cmath>

namespace CircleSettings
{
    float totalRadians{2*3.14159265f};
}


namespace Gravity
{
    int g = 9;
    const float G = 0.6;
}

float cos(float theta)
{
    if (theta > 3.14159265f) theta -= 6.2831853f;
    if (theta < -3.14159265f) theta += 6.2831853f;

    // Taylor cabbage
    float term1 { 1.0f };
    float theta2 {theta * theta};
    float term2 {- theta2 / 2.0f};
    float term4 { (theta2 * theta2) / 24.0f};
    float term6 {- (theta2 * theta2 * theta2) / 720.0f};  
    float term8 = (theta2 * theta2 * theta2 * theta2) / 40320.0f;

    return term1 + term2 + term4 + term6 + term8;
}

float sin(float theta)
{
    if (theta > 3.14159265f) theta -= 6.2831853f;
    if (theta < -3.14159265f) theta += 6.2831853f;

    float term1 { theta };
    float theta2 {theta * theta};
    float term3 {- (theta2 * theta) / 6.0f};
    float term5 { (theta2 * theta2 * theta) / 120.0f};
    float term7 {- (theta2 * theta2 * theta2 * theta) / 5040.0f};  
    float term9 = (theta2 * theta2 * theta2 * theta2 * theta) / 362880.0f;

    return term1 + term3 + term5 + term7 +term9;
}

void drawCircle(float cx, float cy, float r)
{   
    glBegin(GL_TRIANGLE_FAN);   
    glVertex2f(cx,cy); // 0,0
    for (float i {0}; i <= CircleSettings::totalRadians;)
    {
        glVertex2f(cx + r*cos(i), cy + r*sin(i));
        i += 0.01f;
    }
    glEnd();
}

struct Planet {
    std::string_view name{};
    float x, y;
    float vx, vy;
    float r;
    float m;
    float cr, cg, cb;
    float maxVel;
};

std::array <Planet, 4> planets = {{
   //           x     y    vx    vy   r     m    cr    cg    cb
    {"Sun",    0,    0,    0.0,      0.0,   40, 4000, 1.00, 0.78, 0.25,0 }, // sun
    {"Earth",  140,    0,  0.0,   -4.1,   16,   20, 0.35, 0.62, 1.00,0 }, // earth
    {"Mars", -160, -160,   -2.3,    +2.3,  12, 12, 0.90, 0.38, 0.24,0 }, // mars
    {"Fantasy World", -170,  170, -2.2,  +2.2, 14,   14, 0.70, 0.55, 1.00,0 }, // Fantasy world
}};  

int main()
{
    std::cout << "We gonna build a solar system here." << '\n';
    if (!glfwInit())
    {
        std::cerr << "glfw did not start\n";
        return -1; 
    }
    GLFWwindow* window = glfwCreateWindow(800, 600, "Solar system", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "no window for you!\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewInit();

    glfwSwapInterval(1);

    glViewport(0,0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-400, 400, -300, 300, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // for maxVelocity during bouncing and overshoot due to extra KE
    for (auto& p: planets)
    {
        p.maxVel = std::pow(p.vy * p.vy + 2* Gravity::g * std::abs(-300 - p.y), 0.5);
    }
    for (const auto& p: planets)
    {
        std::cout << p.name << ' ' << p.maxVel <<'\n';
    }
    // bouncing stuff loop ends

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& p : planets) 
        {
            // for bouncing pattern
            // if (p.y <= -300)
            // {
            //     p.y = -300;
            //     p.vy = p.maxVel;
            // }
            
            // p.y += p.vy;
            // p.vy -= Gravity::g;
                    float total_ax = 0.0f;
        float total_ay = 0.0f;
        bool collapsedIntoSun = false;

        // Accumulate gravity forces ONLY
        for (auto& other : planets) 
        {
            if (&other == &p) continue;

            float dx = other.x - p.x;
            float dy = other.y - p.y;
            float rSquared = dx * dx + dy * dy;

            // softening clamp to keep math safe during approach 
            if (rSquared < 1600.0f) rSquared = 1600.0f; 

            float a_magnitude = (Gravity::G * other.m) / rSquared;
            float r = std::sqrt(rSquared);

            total_ax += a_magnitude * (dx / r);
            total_ay += a_magnitude * (dy / r);

            if (other.name == planets[0].name) {
                if (r <= (p.r + other.r)) {
                    collapsedIntoSun = true;
                }
            }
        }

        if (collapsedIntoSun) 
        {
            p.vx = planets[0].vx;
            p.vy = planets[0].vy;

            float dx = planets[0].x - p.x;
            float dy = planets[0].y - p.y;
            float r = std::sqrt(dx * dx + dy * dy);
            if (r < 0.1f) r = 0.1f;
    
            float contactDist = p.r + planets[0].r;
            p.x = planets[0].x - (dx / r) * contactDist;
            p.y = planets[0].y - (dy / r) * contactDist;
        } 
        else 
        {
            p.vx += total_ax;
            p.vy += total_ay;
            p.x += p.vx;
            p.y += p.vy;
        }

        // Render step
        glColor3f(p.cr, p.cg, p.cb);
        drawCircle(p.x, p.y, p.r);

        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
