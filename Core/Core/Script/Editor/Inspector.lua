local function DrawTransform(entity)
		if not entity then
			return
		end

		if Interface.CollapsingHeader("Transform") then
			local transform = entity:GetComponent("Transform")
			if not transform then
				return
			end	

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

		if Interface.CollapsingHeader("Mesh Render") then
			local meshRender = entity:GetComponent("MeshRender")
			if not meshRender then
				return
			end

			local names = ResourceManager.GetResourcesName("Material")
			local currentName = ResourceManager.GetResourceName("Material", meshRender.GetMaterialID())

			Interface.Text("Material : ")
			Interface.SameLine()
			Interface.Combo("##MR", currentName, names)
		end
	end

	local function DrawMeshFilter(entity)
		if not entity then
			return
		end

		if Interface.CollapsingHeader("Mesh Filter") then
			local meshFilter = entity:GetComponent("Mesh Filter")
			if not meshFilter then
				return
			end

			local names = MeshFilterPooler.GetMeshFiltersName()

			Interface.Text("Filter : ")
			Interface.SameLine()
			Interface.Combo("##MF", meshFilter.name, names)
		end
	end

	local function DrawDirectionalLight(entity)
		if not entity then
			return
		end

		if Interface.CollapsingHeader("Directional Light") then
			local directionalLight = entity:GetComponent("DirectionalLight")
			if not directionalLight then
				return
			end

			local dirty = false

			Interface.Text("Direction : ")
			Interface.SameLine()
			dirty = Interface.Vec3(directionalLight.direction) or dirty

			Interface.Text("Ambient	  : ")
			Interface.SameLine()
			dirty = Interface.Color3(directionalLight.ambient) or dirty

			Interface.Text("Diffuse   : ")
			Interface.SameLine()
			dirty = Interface.Color3(directionalLight.diffuse) or dirty

			Interface.Text("Specular  : ")
			Interface.SameLine()
			dirty = Interface.Color3(directionalLight.specular)
			
			if dirty then
				Scene.ECS:SetComponentIsDirty("DirectionalLight", true)
			end
		end
	end

	local function DrawPointLight(entity)
		if not entity then 
			return
		end

		if Interface.CollapsingHeader("Point Light") then
			local pointLight = entity:GetComponent("PointLight")
			if not pointLight then
				return
			end

			local dirty = false

			Interface.Text("Position  : ")
			Interface.SameLine()
			dirty = Interface.Vec3(pointLight.position) or dirty

			Interface.Text("Ambient   : ")
			Interface.SameLine()
			dirty = Interface.Color3(pointLight.ambient) or dirty

			Interface.Text("Diffuse   : ")
			Interface.SameLine()
			dirty = Interface.Color3(pointLight.diffuse) or dirty
			
			Interface.Text("Specular  : ")
			Interface.SameLine()
			dirty = Interface.Color3(pointLight.specular) or dirty

			Interface.Text("Constant  : ")
			Interface.SameLine()
			dirty = Interface.Float(pointLight.constant) or dirty

			Interface.Text("Linear    : ")
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

CustomEditor("Inspector",
{
	Draw = function(entity)

		if not entity then
			return
		end

		DrawTransform(entity)
		DrawMeshRender(entity)
		DrawMeshFilter(entity)
		DrawDirectionalLight(entity)
		DrawPointLight(entity)
	end

})