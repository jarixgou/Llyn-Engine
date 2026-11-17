function DrawTree(entity, count)
	Interface.NextRow()
	Interface.NextColumn()

	local sameEntity = false
	if Editor:HasSelectedEntity then
		if Editor.selectedEntity == entity then
			sameEntity = true
		end
	end

	Interface.Tree(entity.name, sameEntity, count)

	local childs = entity:GetChilds()
	for i = 0, childs:size() - 1 do
		DrawTree(childs[i], childs:size())
	end
end

Script("CustomEditor",
{
	title = "Hierarchy"
	menu = "Editor/Hierarchy"

	Draw = function()
		if Interface.IsHovered() and Input.MousePressed("Right") then
			Interface.OpenPopup("NewEntity")
		end

		if Interface.BeginPopup("NewEntity") then
			-- TODO : create new entity
		end

		Interface.BeginTable(1)

		local entities = Scene:GetEntities()
		for i = 0, entities:size() - 1 do
			DrawTree(entities[i], entities:size())
		end

		Interface.EndTable()
	end
})