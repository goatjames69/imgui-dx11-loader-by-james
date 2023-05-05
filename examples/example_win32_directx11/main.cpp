#include "main.h"

HWND hwnd;
RECT rc;

using namespace KeyAuth;
std::string name = ""; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = ""; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = ""; // app secret, the blurred text on licenses tab and other tabs
std::string version = ""; // leave alone unless you've changed version on website
std::string url = "https://keyauth.win/api/1.2/";
KeyAuth::api KeyAuthApp(name, ownerid, secret, version, url);
bool authenticed = false;

bool button = false;

void move_window() {

    ImGui::SetCursorPos(ImVec2(0, 0));
    if (ImGui::InvisibleButton("Move_detector", ImVec2(800, 570)));
    if (ImGui::IsItemActive()) {

        GetWindowRect(hwnd, &rc);
        MoveWindow(hwnd, rc.left + ImGui::GetMouseDragDelta().x, rc.top + ImGui::GetMouseDragDelta().y, 800, 570, TRUE);
    }

}

void RenderBlur(HWND hwnd)
{
    struct ACCENTPOLICY
    {
        int na;
        int nf;
        int nc;
        int nA;
    };
    struct WINCOMPATTRDATA
    {
        int na;
        PVOID pd;
        ULONG ul;
    };

    const HINSTANCE hm = LoadLibrary(L"user32.dll");
    if (hm)
    {
        typedef BOOL(WINAPI* pSetWindowCompositionAttribute)(HWND, WINCOMPATTRDATA*);

        const pSetWindowCompositionAttribute SetWindowCompositionAttribute = (pSetWindowCompositionAttribute)GetProcAddress(hm, "SetWindowCompositionAttribute");
        if (SetWindowCompositionAttribute)
        {
            ACCENTPOLICY policy = { 3, 0, 0, 0 }; // and even works 4,0,155,0 (Acrylic blur)
            WINCOMPATTRDATA data = { 19, &policy,sizeof(ACCENTPOLICY) };
            SetWindowCompositionAttribute(hwnd, &data);
        }
        FreeLibrary(hm);
    }
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    KeyAuthApp.init();
    if (!KeyAuthApp.data.success)
    {
        MessageBoxA(NULL, KeyAuthApp.data.message.c_str(), NULL, NULL);
        exit(0);
    }
    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = NULL;
    wc.cbWndExtra = NULL;
    wc.hInstance = nullptr;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = L"ImGui";
    wc.lpszClassName = L"Example";
    wc.hIconSm = LoadIcon(0, IDI_APPLICATION);

    RegisterClassExW(&wc);
    hwnd = CreateWindowExW(NULL, wc.lpszClassName, L"Example", WS_POPUP, (GetSystemMetrics(SM_CXSCREEN) / 2) - (800 / 2), (GetSystemMetrics(SM_CYSCREEN) / 2) - (570 / 2), 800, 570, 0, 0, 0, 0);
    RenderBlur(hwnd);
    SetWindowLongA(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

    MARGINS margins = { -1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    POINT mouse;
    rc = { 0 };
    GetWindowRect(hwnd, &rc);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }


    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    io.Fonts->AddFontFromMemoryTTF(&inter, sizeof inter, 23, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 38, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ico_logo = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 55, NULL, io.Fonts->GetGlyphRangesCyrillic());
    tab_text = io.Fonts->AddFontFromMemoryTTF(&inter, sizeof inter, 19, NULL, io.Fonts->GetGlyphRangesCyrillic());
    ico_minimize = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 27, NULL, io.Fonts->GetGlyphRangesCyrillic());


    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        static bool hide = true;
        static int opticaly = 255;
        if (GetAsyncKeyState(VK_INSERT) & 0x1) hide = !hide;
        if (GetAsyncKeyState(VK_DELETE) & 1) { exit(0); }
        opticaly = ImLerp(opticaly, opticaly <= 255 && hide ? 300 : 0, ImGui::GetIO().DeltaTime * 8.f);
        if (opticaly > 255) opticaly = 255;
        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), opticaly, LWA_ALPHA);

        ShowWindow(hwnd, opticaly > 0 ? SW_SHOW : SW_HIDE);
        ImGui::NewFrame();
        if (authenticed == false) {
            {
                CustomStyleColor();
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(ImVec2(800, 570));

                ImGui::Begin("Inicio de Sesion", &menu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
                {
                    const auto& p = ImGui::GetWindowPos();
                    ImGuiStyle& s = ImGui::GetStyle();

                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(20 + p.x, 90 + p.y), ImVec2(80 + p.x, 91 + p.y), ImColor(190, 200, 210, 90), s.WindowRounding);

                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(20 + p.x, 462 + p.y), ImVec2(80 + p.x, 463 + p.y), ImColor(190, 200, 210, 90), s.WindowRounding);

                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0 + p.x, 0 + p.y), ImVec2(800 + p.x, 570 + p.y), ImGui::GetColorU32(ImGuiCol_WindowBg), s.WindowRounding);

                    ImGui::GetWindowDrawList()->AddRect(ImVec2(0 + p.x, 0 + p.y), ImVec2(800 + p.x, 570 + p.y), ImGui::GetColorU32(ImGuiCol_Border), s.WindowRounding);

                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0 + p.x, 0 + p.y), ImVec2(100 + p.x, 570 + p.y), colors::lite_color, s.WindowRounding, ImDrawFlags_RoundCornersLeft);

                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(100 + p.x, 0 + p.y), ImVec2(101 + p.x, 570 + p.y), ImColor(35, 35, 35, 200), s.WindowRounding);

                    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(120 + p.x, 10 + p.y), ImVec2(185 + p.x, 75 + p.y), colors::lite_color, s.WindowRounding, ImDrawFlags_RoundCornersLeft);

                    ImGui::GetWindowDrawList()->AddRect(ImVec2(120 + p.x, 10 + p.y), ImVec2(440 + p.x, 75 + p.y), ImColor(35, 35, 35, 200), s.WindowRounding);

                    // logo & user

                    ImGui::GetWindowDrawList()->AddImage(us, ImVec2(21 + p.x, 490 + p.y), ImVec2(81 + p.x, 550 + p.y), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));

                    ImGui::GetWindowDrawList()->AddText(ico_logo, 55, ImVec2(30 + p.x, 22 + p.y), ImColor(80, 198, 212, 255), "J");

                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 25));

                    ImGui::BeginGroupPos(ImVec2(16, 103));
                    {
                        if (ImGui::Tab("H", 0 == tabs, ImVec2(50, 47))) tabs = 0;

                        if (ImGui::Tab("G", 1 == tabs, ImVec2(47, 47))) tabs = 1;
                    }
                    ImGui::PopStyleVar();

                    tab_alpha = ImClamp(tab_alpha + (7.f * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);
                    tab_add = ImClamp(tab_add + (std::round(50.f) * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);

                    if (tab_alpha == 0.f && tab_add == 0.f)
                        active_tab = tabs;

                    ImGui::EndGroupPos();

                    ImGui::SetCursorPos(ImVec2(120, 90));

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * s.Alpha);

                    switch (active_tab) {
                    case 0:
                    {

                        ImGui::GetWindowDrawList()->AddText(ico, 38, ImVec2(137 + p.x, 25 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "H");

                        ImGui::GetWindowDrawList()->AddText(tab_text, 19, ImVec2(200 + p.x, 23 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "Login Page\nEnter your details to proceed");

                        ImGui::BeginChild("Login", ImVec2(320, 460), true);
                        {
                            ImGui::InputText("Username", username, 64);
                            ImGui::InputText("Password", pass, 64);
                            if (ImGui::Button("Login", ImVec2(200, 40)))
                            {
                                KeyAuthApp.login(username, pass);
                                if (!KeyAuthApp.data.success)
                                {
                                    MessageBoxA(NULL, KeyAuthApp.data.message.c_str(), NULL, NULL);

                                }
                                else authenticed = true;
                            }

                        }
                    }
                    break;

                    case 1:
                    {

                        ImGui::GetWindowDrawList()->AddText(ico, 38, ImVec2(138 + p.x, 25 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "G");

                        ImGui::GetWindowDrawList()->AddText(tab_text, 19, ImVec2(200 + p.x, 23 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "Register\nRegister your new accounts.");

                        ImGui::BeginChild("Register", ImVec2(320, 460), true);
                        {
                            ImGui::InputText("Username", username, 64);
                            ImGui::InputText("Password", pass, 64);
                            ImGui::InputText("License", key, 64);
                            if (ImGui::Button("Register", ImVec2(200, 40)))
                            {
                                KeyAuthApp.regstr(username, pass, key);
                                if (!KeyAuthApp.data.success)
                                {
                                    MessageBoxA(NULL, KeyAuthApp.data.message.c_str(), NULL, NULL);

                                }
                                else authenticed = true;
                            }

                        }
                    }
                    break;

                    }

                    ImGui::Spacing();

                }
                ImGui::EndChild();

                ImGui::PopStyleVar();
                move_window();

                RenderBlur(hwnd);
                ImGui::End();
            }

        }

        if (authenticed == true)
        {



            D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
            if (us == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, user, sizeof(user), &info, pump, &us, 0);

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(800, 570));
            ImGui::Begin("Menu", &menu, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground);
            {

                const auto& p = ImGui::GetWindowPos();

                ImGuiStyle& s = ImGui::GetStyle();

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(20 + p.x, 90 + p.y), ImVec2(80 + p.x, 91 + p.y), ImColor(190, 200, 210, 90), s.WindowRounding);

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(20 + p.x, 462 + p.y), ImVec2(80 + p.x, 463 + p.y), ImColor(190, 200, 210, 90), s.WindowRounding);

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0 + p.x, 0 + p.y), ImVec2(800 + p.x, 570 + p.y), ImGui::GetColorU32(ImGuiCol_WindowBg), s.WindowRounding);

                ImGui::GetWindowDrawList()->AddRect(ImVec2(0 + p.x, 0 + p.y), ImVec2(800 + p.x, 570 + p.y), ImGui::GetColorU32(ImGuiCol_Border), s.WindowRounding);

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(0 + p.x, 0 + p.y), ImVec2(100 + p.x, 570 + p.y), colors::lite_color, s.WindowRounding, ImDrawFlags_RoundCornersLeft);

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(100 + p.x, 0 + p.y), ImVec2(101 + p.x, 570 + p.y), ImColor(35, 35, 35, 200), s.WindowRounding);

                ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(120 + p.x, 10 + p.y), ImVec2(185 + p.x, 75 + p.y), colors::lite_color, s.WindowRounding, ImDrawFlags_RoundCornersLeft);

                ImGui::GetWindowDrawList()->AddRect(ImVec2(120 + p.x, 10 + p.y), ImVec2(440 + p.x, 75 + p.y), ImColor(35, 35, 35, 200), s.WindowRounding);

                // logo & user

                ImGui::GetWindowDrawList()->AddImage(us, ImVec2(21 + p.x, 490 + p.y), ImVec2(81 + p.x, 550 + p.y), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));

                ImGui::GetWindowDrawList()->AddText(ico_logo, 55, ImVec2(30 + p.x, 22 + p.y), ImColor(80, 198, 212, 255), "J");

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 25));

                ImGui::BeginGroupPos(ImVec2(16, 103));
                {
                    if (ImGui::Tab("A", 0 == tabs, ImVec2(50, 47))) tabs = 0;

                    if (ImGui::Tab("C", 2 == tabs, ImVec2(50, 47))) tabs = 2;

                    if (ImGui::Tab("F", 3 == tabs, ImVec2(51, 47))) tabs = 3;

                    if (ImGui::Tab("E", 4 == tabs, ImVec2(49, 47))) tabs = 4;

                }

                ImGui::PopStyleVar();

                tab_alpha = ImClamp(tab_alpha + (7.f * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);
                tab_add = ImClamp(tab_add + (std::round(50.f) * ImGui::GetIO().DeltaTime * (tabs == active_tab ? 1.f : -1.f)), 0.f, 1.f);

                if (tab_alpha == 0.f && tab_add == 0.f)
                    active_tab = tabs;

                ImGui::EndGroupPos();

                ImGui::SetCursorPos(ImVec2(120, 90));

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, tab_alpha * s.Alpha);

                switch (active_tab) {

                case 0:

                    ImGui::GetWindowDrawList()->AddText(ico, 38, ImVec2(137 + p.x, 25 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "A");

                    ImGui::GetWindowDrawList()->AddText(tab_text, 19, ImVec2(200 + p.x, 23 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "Aimbot\nMakes your Aim Perfect");

                    ImGui::BeginChild("Aimbot", ImVec2(320, 460), true);
                    {
                      
                        ImGui::Checkbox("Button", &button);

                    }
                    ImGui::EndChild();

                    break;


                case 2:

                    ImGui::GetWindowDrawList()->AddText(ico, 38, ImVec2(137 + p.x, 23 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "C");

                    ImGui::GetWindowDrawList()->AddText(tab_text, 19, ImVec2(200 + p.x, 23 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "Visuales\nShows location of enemy.");

                    ImGui::BeginChild("Visual", ImVec2(320, 460), true);
                    {
                      
                        ImGui::Checkbox("Button", &button);

                    }
                    ImGui::EndChild();


                    break;

                case 3:

                    ImGui::GetWindowDrawList()->AddText(ico, 38, ImVec2(137 + p.x, 25 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "D");

                    ImGui::GetWindowDrawList()->AddText(tab_text, 19, ImVec2(200 + p.x, 23 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "Misc\nExtra Options.");

                    ImGui::BeginChild("Misc", ImVec2(320, 460), true);
                    {
                      
                        ImGui::Checkbox("Button", &button);

                    }
                    ImGui::EndChild();

                    break;

                case 4:

                    ImGui::GetWindowDrawList()->AddText(ico, 38, ImVec2(137 + p.x, 23 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "E");

                    ImGui::GetWindowDrawList()->AddText(tab_text, 19, ImVec2(200 + p.x, 23 + p.y), ImGui::GetColorU32(colors::tab_tooltip), "Bypass\nSecure your accounts.");

                    ImGui::BeginChild("Child##0", ImVec2(320, 460), true);
                    {
                        
                        ImGui::Checkbox("Button", &button);

                    }
                    ImGui::EndChild();

                    break;

                }

                ImGui::Spacing();
            }

            ImGui::PopStyleVar();
            move_window();
            RenderBlur(hwnd);
            ImGui::End();
        }
      
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

