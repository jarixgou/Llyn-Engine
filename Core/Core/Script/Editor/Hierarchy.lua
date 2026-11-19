function DrawTree(entity, count)
	Interface.NextRow()
	Interface.NextColumn()

	local sameEntity = false
	if Editor:HasSelectedEntity() then
		if Editor.selectedEntity:GetID() == entity:GetID() then
			sameEntity = true
		end
	end

	local isOpen = Interface.Tree(entity.name, sameEntity, count)

	local clicked = Interface.IsItemClicked or Interface.IsItemFocused

	if clicked then
		Editor:SetSelectedEntity(entity)
	end

	local childs = entity:GetChilds()

	if childs:size() == 0 then
		isOpen = false
	end

	if isOpen then
		for i = 0, childs:size() - 1 do
			DrawTree(childs[i], childs:size())
		end
	end
end

Script("CustomEditor", "Hierarchy",
{
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