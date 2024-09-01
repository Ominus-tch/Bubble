#include "SceneHierarchyPanel.h"

#include "Bubble/Scene/Components.h"
#include "Bubble/Utils/PlatformUtils.h"

#include "Bubble/Scripting/ScriptEngine.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

#include <cstring>

#include <filesystem>

#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

namespace Bubble {
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
				{
					Entity entity{ entityID, m_Context.get() };
					DrawEntityNode(entity);
				});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			// Right-click on blank space
			if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
					m_Context->CreateEntity("Empty Entity");

				ImGui::EndPopup();
			}
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		static std::map<uint32_t, bool> selectionPending;

		auto& tag = entity.GetComponent<TagComponent>().Tag;
		uint32_t entityID = (uint32_t)entity;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)entityID, flags, entity.GetName().c_str());

		bool itemClicked = ImGui::IsItemClicked(ImGuiMouseButton_Left);
		bool isDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);

		if (itemClicked && !isDragging)
		{
			// Set the flag indicating that we intend to select this item
			selectionPending[entityID] = true;
		}

		// Start drag-and-drop operation
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("ENTITY", &entityID, sizeof(uint32_t));

			// Reset the selection flag because we're dragging, not selecting
			selectionPending[entityID] = false;

			// Optional: ImGui::Image((ImTextureID)icon->GetRendererID(), { thumbnailSize * 0.2f, thumbnailSize * 0.2f }, { 0, 1 }, { 1, 0 });

			ImGui::EndDragDropSource();
		}

		// If the mouse button was released and we were not dragging, select the item
		if (selectionPending[entityID] && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			m_SelectionContext = entity;
			selectionPending[entityID] = false; // Reset the selection flag
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGui::Text("%d", entityID);
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		bool changed = false;

		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize)) {
			values.x = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize)) {
			values.y = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize)) {
			values.z = resetValue;
			changed = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		changed |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();

		return changed;
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy_s(buffer, sizeof(buffer), tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DisplayAddComponentEntry<MeshComponent>("Mesh");
			//DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			//DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			//DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			//DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");
			//DisplayAddComponentEntry<TextComponent>("Text Component");

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}
			});

		DrawComponent<ScriptComponent>("Script", entity, [entity, scene = m_Context](ScriptComponent& component) mutable
			{
				bool scriptClassExists = ScriptEngine::EntityClassExists(component.ClassName);

				static char buffer[64];
				strcpy_s(buffer, sizeof(buffer), component.ClassName.c_str());

				//UI::ScopedStyleColor textColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.3f, 1.0f), !scriptClassExists);

				if (ImGui::InputText("Class", buffer, sizeof(buffer)))
				{
					component.ClassName = buffer;
					return;
				}
				if (ImGui::BeginDragDropTarget())
				{
					bool ret = false;
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path scriptPath(path);
						std::string fullScriptName = Utils::ExtractNamespaceClass(scriptPath.string());
						if (!fullScriptName.empty() && component.ClassName != fullScriptName)
						{
							component.ClassName = fullScriptName;
							ret = true;
						}
					}
					ImGui::EndDragDropTarget();
					if (ret) return;
				}

				// Fields
				bool sceneRunning = scene->IsRunning();
				if (sceneRunning)
				{
					Ref<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
					if (scriptInstance)
					{
						const auto& fields = scriptInstance->GetScriptClass()->GetFields();
						for (const auto& [name, field] : fields)
						{
							switch (field.Type) {
								case ScriptFieldType::Float: {

									float data = scriptInstance->GetFieldValue<float>(name);
									if (ImGui::DragFloat(name.c_str(), &data))
									{
										scriptInstance->SetFieldValue(name, data);
									}

									break;
								}
								case ScriptFieldType::Int: {

									int data = scriptInstance->GetFieldValue<int>(name);
									if (ImGui::DragInt(name.c_str(), &data))
									{
										scriptInstance->SetFieldValue(name, data);
									}

									break;
								}
								case ScriptFieldType::Vector3: {
									glm::vec3 data = scriptInstance->GetFieldValue<glm::vec3>(name);
									if (DrawVec3Control(name, data))
									{
										scriptInstance->SetFieldValue(name, data);
									}
									break;
								}
								case ScriptFieldType::Entity: {
									uint32_t data = scriptInstance->GetFieldValue<uint32_t>(name);
									std::ostringstream entityLabel;
									entityLabel << "Entity: ";
									if (data != 0)
									{
										Entity entity(data, scene.get());
										entityLabel << entity.GetName();
									}
									else
									{
										entityLabel << "None";
									}

									ImGui::Button(entityLabel.str().c_str());

									if (ImGui::BeginDragDropTarget())
									{
										if (data != 0)
											ImGui::SetDragDropPayload("ENTITY", &data, sizeof(uint32_t));
										if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
										{
											uint32_t entityID = *(const uint32_t*)payload->Data;
											if (entityID != data)
												scriptInstance->SetFieldValue(name, entityID);
										}
										ImGui::EndDragDropTarget();
									}

									break;
									
								}
								default: break;
							}
						}
					}
				}
				else
				{
					if (scriptClassExists)
					{
						Ref<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
						const auto& fields = entityClass->GetFields();

						auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);
						for (const auto& [name, field] : fields)
						{
							// Field has been set in editor
							if (entityFields.find(name) != entityFields.end())
							{
								ScriptFieldInstance& scriptField = entityFields.at(name);

								// Display control to set it maybe
								switch (field.Type) {
									case ScriptFieldType::Float: {
										float data = scriptField.GetValue<float>();
										if (ImGui::DragFloat(name.c_str(), &data))
											scriptField.SetValue(data);
										break;
									}
									case ScriptFieldType::Int: {
										int data = scriptField.GetValue<int>();
										if (ImGui::DragInt(name.c_str(), &data))
											scriptField.SetValue(data);
										break;
									}
									case ScriptFieldType::Vector3: {
										glm::vec3 data = scriptField.GetValue<glm::vec3>();
										if (DrawVec3Control(name, data))
										{
											scriptField.SetValue(data);
										}
										break;
									}
									case ScriptFieldType::Entity: {
										uint32_t data = scriptField.GetValue<uint32_t>();
										std::ostringstream entityLabel;
										Entity entity(data, scene.get());
										entityLabel << name << ": " << entity.GetName();

										ImGui::Button(entityLabel.str().c_str());

										if (ImGui::BeginDragDropTarget())
										{
											if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
											{
												uint32_t entityID = *(const uint32_t*)payload->Data;
												if (data != entityID)
													scriptField.SetValue(entityID);
											}
											ImGui::EndDragDropTarget();
										}
										break;
									}
									default: break;
								}
							}
							else
							{
								// Display control to set it maybe
								switch (field.Type) {
									case ScriptFieldType::Float: {
										float data = 0.0f;
										if (ImGui::DragFloat(name.c_str(), &data))
										{
											ScriptFieldInstance& fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(data);
										}
										break;
									}
									case ScriptFieldType::Int: {
										int data = 0;
										if (ImGui::DragInt(name.c_str(), &data))
										{
											ScriptFieldInstance& fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(data);
										}
										break;
									}
									case ScriptFieldType::Vector3: {
										glm::vec3 data = {0.f, 0.f, 0.f};
										if (DrawVec3Control(name, data))
										{
											ScriptFieldInstance& fieldInstance = entityFields[name];
											fieldInstance.Field = field;
											fieldInstance.SetValue(data);
										}
										break;
									}
									case ScriptFieldType::Entity: {
										std::ostringstream entityLabel;
										entityLabel << name.c_str() << ": None";
										ImGui::Button(entityLabel.str().c_str());
										if (ImGui::BeginDragDropTarget())
										{
											if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY"))
											{
												uint32_t entityID = *(const uint32_t*)payload->Data;
												ScriptFieldInstance& fieldInstance = entityFields[name];
												fieldInstance.Field = field;
												fieldInstance.SetValue(entityID);
											}
											ImGui::EndDragDropTarget();
										}
										break;
									}
									default: break;
								}
							}
						}
					}
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				if (ImGui::Button("Texture", ImVec2(100.0f, 0.0f)))
				{
					std::string filepath = FileDialogs::OpenFile("Texture File (*.png)\0*.png\0");
					if (!filepath.empty())
					{
						Ref<Texture2D> texture = Texture2D::Create(filepath);
						if (texture->IsLoaded())
							component.Texture = texture;
						else
							BG_WARN("Could not load texture {0}", filepath);
					}
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath(path);
						Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
						if (texture->IsLoaded())
							component.Texture = texture;
						else
							BG_WARN("Could not load texture {0}", texturePath.filename().string());
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			});

		DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent& component)
			{
				ImGui::Text("Path: %s", component.Path.c_str());
				if (ImGui::Button("Model", ImVec2(100.0f, 0.0f)))
				{
					std::string filepath = FileDialogs::OpenFile("FBX Model (*.fbx)\0*.fbx\0");
					if (!filepath.empty())
					{
						component.Load(filepath);
					}
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path modelPath(path);
						component.Load(modelPath.string());
						//BG_WARN("Could not load texture {0}", texturePath.filename().string());
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::Checkbox("Draw Mesh", &component.DrawMesh);
				ImGui::Checkbox("Draw Wireframe", &component.DrawWireframe);

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;

				for (int i = 0; i < component.Meshes.size(); i++)
				{
					const auto& mesh = component.Meshes[i];
					if (ImGui::TreeNodeEx((void*)mesh.get(), flags, "Mesh Part #%d", i))
					{
						glm::vec3 translation = mesh->GetTranslation();
						glm::vec3 rotation = glm::degrees(mesh->GetRotation());
						glm::vec3 scale = mesh->GetScale();

						DrawVec3Control("Translation", translation);
						DrawVec3Control("Rotation", rotation);
						DrawVec3Control("Scale", scale, 1.0f);

						ImGui::Text("Vertices: %d", mesh->GetNumVertices());
						ImGui::Text("Indices: %d", mesh->GetNumIndices());

						mesh->updateTransform(translation, rotation, scale);

						ImGui::TreePop();
					}
				}
			});
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}
}
