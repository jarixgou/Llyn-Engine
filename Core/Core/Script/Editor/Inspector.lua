local function DrawTransform(entity)
	if not entity then
		return
	end

	local transform = entity:GetComponent("Transform")
	if not transform then
		return
	end	

	if Interface.CollapsingHeader("Transform") then
		Interface.Text("Position : ")
		Interface.SameLine()
		Interface.Vec3(transform.position)

		Interface.Text("Rotation : ")
		Interface.SameLine()
		local rot = transform.rotation:GetEuler()
		Interface.Vec3(rot)
		transform.rotation:SetEuler(rot)

		Interface.Text("Scale    : ")
		Interface.SameLine()
		Interface.Vec3(transform.scale)
	end
end

local function DrawMeshRender(entity)
	if not entity then
		return
	end

	local meshRender = entity:GetComponent("MeshRender")
	if not meshRender then
		return
	end

	if Interface.CollapsingHeader("Mesh Render") then
		local names = ResourceManager.GetResourcesName("Material")
		local currentName = ResourceManager.GetResourceName("Material", meshRender.GetMaterialID())

		Interface.Text("Material : ")
		Interface.SameLine()
		Interface.Combo(currentName, names)
	end
end

local function DrawMeshFilter(entity)
	if not entity then
		return
	end

	local meshFilter = entity:GetComponent("MeshFilter")
	if not meshFilter then
		return
	end

	if Interface.CollapsingHeader("Mesh Filter") then
		local names = MeshFilterPooler.GetMeshFiltersName()

		Interface.Text("Filter : ")
		Interface.SameLine()
		Interface.Combo("##MF", meshFilter.name, names)
	end
end

function DrawDirectionalLight(entity)
	if not entity then
		return
	end

	local directionalLight = entity:GetComponent("DirectionalLight")
	if not directionalLight then
		return
	end

	if Interface.CollapsingHeader("Directional Light") then
		local dirty = false

		Interface.Text("Direction : ")
		Interface.SameLine()
		dirty = Interface.Vec3(directionalLight.direction) or dirty

		Interface.Text("Ambient	: ")
		Interface.SameLine()
		dirty = Interface.Color3(directionalLight.ambient) or dirty

		Interface.Text("Diffuse : ")
		Interface.SameLine()
		dirty = Interface.Color3(directionalLight.diffuse) or dirty

		Interface.Text("Specular : ")
		Interface.SameLine()
		dirty = Interface.Color3(directionalLight.specular)
		
		if dirty then
			Scene.ECS:SetComponentIsDirty("DirectionalLight", true)
		end
	end
end

function DrawPointLight(entity)
	if not entity then 
		return
	end

	local pointLight = entity:GetComponent("PointLight")
	if not pointLight then
		return
	end

	if Interface.CollapsingHeader("Point Light") then
		local dirty = false

		Interface.Text("Position : ")
		Interface.SameLine()
		dirty = Interface.Vec3(pointLight.position) or dirty

		Interface.Text("Ambient : ")
		Interface.SameLine()
		dirty = Interface.Color3(pointLight.ambient) or dirty

		Interface.Text("Diffuse : ")
		Interface.SameLine()
		dirty = Interface.Color3(pointLight.diffuse) or dirty
		
		Interface.Text("Specular : ")
		Interface.SameLine()
		dirty = Interface.Color3(pointLight.specular) or dirty

		Interface.Text("Constant : ")
		Interface.SameLine()
		dirty = Interface.Float(pointLight.constant) or dirty

		Interface.Text("Linear : ")
		Interface.SameLine()
		dirty = Interface.Float(pointLight.linear) or dirty

		Interface.Text("Quadratic : ")
		Interface.SameLine()
		dirty = Interface.Float(pointLight.quadratic) or dirty

		if dirty then
			Scene.ECS:SetComponentIsDirty("PointLight", true)
		end
	end   
end

function DrawSpotLight(entity)
	if not entity then
		return
	end

	local spotLight = entity:GetComponent("SpotLight")
	if not spotLight then
		return
	end

	if Interface.CollapsingHeader("Spot Light") then
		local dirty = false	

		Interface.Text("Position : ")
		Interface.SameLine()
		dirty = Interface.Vec3(spotLight.position) or dirty

		Interface.Text("Direction : ")
		Interface.SameLine()
		dirty = Interface.Vec3(spotLight.direction) or dirty

		Interface.Text("Ambient : ")
		Interface.SameLine()
		dirty = Interface.Color3(spotLight.ambient) or dirty

		Interface.Text("Diffuse : ")
		Interface.SameLine()
		dirty = Interface.Color3(spotLight.diffuse) or dirty

		Interface.Text("Specular : ")
		Interface.SameLine()
		dirty = Interface.Color3(spotLight.specular)

		Interface.Text("Cut Off : ")
		Interface.SameLine()
		dirty = Interface.Float(spotLight.cutOff)

		Interface.Text("Outer Cut Off : ")
		Interface.SameLine()
		dirty = Interface.Float(spotLight.outerCutOff)
	end
end

function AddComponent(entity)
	if not entity then
		return
	end

	Interface.SetCursorPosX(Interface.GetItemRectSize().x * 0.5 - 50)
	
	if Interface.Button("Add Component") then
		Interface.OpenPopup("AddComponent")
	end

	if Interface.BeginPopup("AddComponent") then
		if Interface.MenuItem("Mesh Render") then
			entity:AddComponent("MeshRender")
		end

		if Interface.MenuItem("Mesh Filter") then
			entity:AddComponent("MeshFilter")
		end

		if Interface.MenuItem("Directional Light") then
			entity:AddComponent("DirectionalLight")
		end

		if Interface.MenuItem("Point Light") then
			entity:AddComponent("PointLight")
		end

		if Interface.MenuItem("Spot Light") then
			entity:AddComponent("SpotLight")
		end

		Interface.EnPopup()
	end
end

Script("CustomEditor",
{
	title = "Inspector"
	menu = "Editor/Inspector"

	Draw = function()
		if not Editor:HasEntitySelected then
			return
		end

		local entity = Editor.selectedEntity

		DrawTransform(entity)
		DrawMeshRender(entity)
		DrawMeshFilter(entity)
		DrawDirectionalLight(entity)
		DrawPointLight(entity)
		DrawSpotLight(entity)

		Interface.Separator()

		AddComponent(entity)

	end
})