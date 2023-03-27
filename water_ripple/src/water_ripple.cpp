// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include <tuple>
#include <vector>
#include <math.h>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <memory>
#include <algorithm>

//---------------------------------------------------------------
#define W 500
#define H 500
float factor = 0.99f;

int w = W;
int h = H;
bool flagX = true;

//---------------------------------------------------------------
float randomInit()
{
	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_real_distribution<float> dist(0.0f, w);
	return dist(engine);
}

//---------------------------------------------------------------

int randomInitPos()
{
	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_int_distribution<int> dist(1, W - 1);
	return dist(engine);
}

//---------------------------------------------------------------

struct Mem
{

	std::vector<std::vector<float>> mem;
	Mem()
	{
		std::vector<float> current_i(W, 0.0f);
		mem = std::vector<std::vector<float>>(H, current_i);
	}
};

//---------------------------------------------------------------
struct Drop
{

	float x;
	float y;
};
//---------------------------------------------------------------

std::tuple<float, float> CheckPosM(float a, float b)
{

	float xi;
	float yi;

	if (a > W || a < 0)
	{
		xi = randomInitPos();
	}
	else
	{
		xi = a;
	}
	if (b > H || b < 0)
	{
		yi = randomInitPos();
	}
	else
	{
		yi = b;
	}

	return std::make_tuple(xi, yi);
}

//---------------------------------------------------------------

int main(int argc, char const *argv[])
{

	std::string title = "Starfield simulation";
	initImgui(w, h, title);
	bool flag = true;

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.0f / 255.0, 0.0f / 0.0, 0.0f / 255.0, 1.00f);
	ImVec4 line_color = ImVec4(255.0f / 255.0, 255.0f / 255.0, 255.0f / 255.0, 1.00f);
	ImVec4 blue_color = ImVec4(0.0f / 255.0, 0.0f / 255.0, 170.0f / 255.0, 1.00f);
	ImVec4 pink_color = ImVec4(179.0f / 255.0, 12.0f / 255.0, 130.0f / 255.0, 1.00f);
	ImVec4 green_color = ImVec4(60.0f / 255.0, 160.0f / 255.0, 90.0f / 255.0, 1.00f);
	ImVec4 dot_color = ImVec4(255.0f / 255.0, 255.0f / 255.0, 255.0f / 255.0, 1.00f);

	std::unique_ptr<Mem> current(new Mem);
	std::unique_ptr<Mem> previous(new Mem);
	std::unique_ptr<Mem> temp(new Mem);

	int randomDrops = 15;
	if (flagX == true)
	{
		for (int ii = 0; ii < randomDrops; ii++)
		{

			current->mem[randomInitPos()][randomInitPos()] = 255.0f;
		}
	}

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		if (flagX == false)
		{
			ImGuiIO io = ImGui::GetIO();
			ImVec2 mouse_posIMgui = io.MousePos;
			Drop drop_pos{mouse_posIMgui.x, mouse_posIMgui.y};

			auto posMouse = CheckPosM(drop_pos.x, drop_pos.y);

			current->mem[(int)std::get<0>(posMouse)][(int)std::get<1>(posMouse)] = 255.0f;
		}
		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Water Ripple", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		for (int ii = 1; ii < current->mem[0].size() - 1; ii++)
		{
			for (int jj = 1; jj < current->mem.size(); jj++)
			{

				current->mem[ii][jj] = (previous->mem[ii][jj - 1] + previous->mem[ii][jj + 1] + previous->mem[ii - 1][jj] + previous->mem[ii + 1][jj]) * 0.5f - current->mem[ii][jj];
				current->mem[ii][jj] = current->mem[ii][jj] * factor;
			}
		}

		for (int ii = 1; ii < current->mem[0].size() - 1; ii++)
		{
			for (int jj = 1; jj < current->mem.size() - 1; jj++)
			{
				auto D = 0.0;
				ImVec2 p0;
				ImVec2 p1;

				p0 = {(float)ii, (float)jj};
				p1 = {(float)ii + 1, (float)jj + 1};

				float pix = current->mem[ii][jj];

				ImVec4 pix_color = ImVec4(0.0f, 0.0f, pix, 1.0f);

				draw_list->AddRectFilled(p0, p1, ImColor(pix_color));
			}
		}

		std::swap(current, previous);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
