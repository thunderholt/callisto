import bpy
import json
import bmesh
import os
import math
from bpy.app.handlers import persistent

bl_info = {"name": "Callisto Tools", "category": "Object"}

#-------------------------------------------------------------------------------------------------
# Class: CallistoConfig
#-------------------------------------------------------------------------------------------------
class CallistoConfig():
	scene_object_enum_string_pairs = []

#-------------------------------------------------------------------------------------------------
# Class: CallistoScenePropertiesPanel
#-------------------------------------------------------------------------------------------------
class CallistoScenePropertiesPanel(bpy.types.Panel):
	bl_label = "Callisto Scene Properties"
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'
	bl_context = "scene"

	def draw(self, context):
		self.layout.prop(bpy.context.scene, "callisto_config_file_path")
		#self.layout.prop(bpy.context.scene, "callisto_asset_folder_path")
		self.layout.prop(bpy.context.scene, "callisto_output_folder_name")
		#self.layout.prop(bpy.context.scene, "callisto_asset_copy_script_path")
		#self.layout.prop(bpy.context.scene, "callisto_pvs_generator_exe_path")
		#self.layout.prop(bpy.context.scene, "callisto_engine_exe_path")
		#self.layout.prop(bpy.context.scene, "callisto_grid_dimensions")
		#self.layout.prop(bpy.context.scene, "callisto_grid_cell_size")
		self.layout.operator("object.callistostaticmeshexport", text="Export Static Mesh")
		self.layout.operator("object.callistomapexport", text="Export Map")
		self.layout.operator("object.callistogeneratepvs", text="Generate PVS")
		self.layout.operator("object.playmapoperator", text="Play Map")
		self.layout.operator("object.callistoclearassetoperator", text="Clear Asset Cache")
		
#-------------------------------------------------------------------------------------------------
# Class: CallistoObjectPropertiesPanel
#-------------------------------------------------------------------------------------------------
class CallistoObjectPropertiesPanel(bpy.types.Panel):
	bl_label = "Callisto Object Properties"
	bl_space_type = 'PROPERTIES'
	bl_region_type = 'WINDOW'
	bl_context = "object"

	def draw(self, context):
		self.layout.prop(context.object, "callisto_role")

		if context.object.callisto_role == "WorldMeshChunk":
			#context.object.callisto_material_asset_enum = context.object.callisto_material_asset_sub_path
			#self.layout.prop(context.object, "callisto_material_asset_sub_path")
			self.layout.prop(context.object, "callisto_material_asset_enum")
		elif context.object.callisto_role == "ActorInstance":
			#self.layout.prop(context.object, "callisto_actor_asset_sub_path")
			self.layout.prop(context.object, "callisto_actor_asset_enum")

			setup_callisto_actor_instance_object_properties_panel(self.layout, context.object)
		elif context.object.callisto_role == "StaticMeshChunk":
			self.layout.prop(context.object, "callisto_material_asset_enum")

#-------------------------------------------------------------------------------------------------
# Class: CallistoExportMapOperator
#-------------------------------------------------------------------------------------------------
class CallistoExportMapOperator(bpy.types.Operator):
	bl_label = "Callisto Map Export Operator"
	bl_idname = "object.callistomapexport"
	bl_description = "Export the scene as a Callisto map."
 
	def execute(self, context):
		exporter = CallistoMapExporter()
		exporter.export()
		return {'FINISHED'}

#-------------------------------------------------------------------------------------------------
# Class: CallistoExportStaticMeshOperator
#-------------------------------------------------------------------------------------------------
class CallistoExportStaticMeshOperator(bpy.types.Operator):
	bl_label = "Callisto Static Mesh Export Operator"
	bl_idname = "object.callistostaticmeshexport"
	bl_description = "Export the scene as a Callisto static mesh."
 
	def execute(self, context):
		exporter = CallistoStaticMeshExporter()
		exporter.export()
		return {'FINISHED'}

#-------------------------------------------------------------------------------------------------
# Class: CallistoGeneratePvsOperator
#-------------------------------------------------------------------------------------------------
class CallistoGeneratePvsOperator(bpy.types.Operator):
	bl_label = "Callisto Generate PVS Operator"
	bl_idname = "object.callistogeneratepvs"
	bl_description = "Generate a Callisto PVS file for the scene."

	def execute(self, context):

		###########################
		#for obj in bpy.context.scene.objects:
		#	if obj.callisto_role == "WorldMeshChunk":
		#		obj.callisto_material_asset_sub_path = "debug/wall-1.mat"
		#return {'FINISHED'}
		###########################

		config = load_callisto_config()
		if config != None:
			base_file_name = bpy.path.basename(bpy.data.filepath)

			worldMeshAssetSubPath = build_callisto_output_asset_sub_path(".wmesh")
			pvsAssetSubPath = build_callisto_output_asset_sub_path(".pvs")

			cmd = "start %s \"%s\" \"%s\" \"%s\"" % (
				config["pvs-generator-exe-path"],
				build_callisto_asset_file_path("world-meshes", worldMeshAssetSubPath),
				config["asset-folder-path"] + "/",
				build_callisto_asset_file_path("pvs", pvsAssetSubPath))
			#filepath = 'start C:\\Users\\andym\\Documents\\GitHub\\Callisto\\Windows\\Build\\PVSBuilder\\Release\\PVSBuilder.exe "C:\\Users\\andym\\Documents\\GitHub\\Callisto\\Assets\\Compiled\\world-meshes\\test.wmesh" "C:\\Users\\andym\\Documents\\GitHub\\Callisto\\Assets\\Compiled\\pvs\\test.pvs"'
			print(cmd)
			os.system(cmd)

		return {'FINISHED'}

#-------------------------------------------------------------------------------------------------
# Class: CallistoPlayMapOperator
#-------------------------------------------------------------------------------------------------
class CallistoPlayMapOperator(bpy.types.Operator):
	bl_label = "Callisto Generate PVS Operator"
	bl_idname = "object.playmapoperator"
	bl_description = "Generate a Callisto PVS file for the scene."

	def execute(self, context):
		config = load_callisto_config()
		if config != None:
			map_asset_ref = "maps/" + build_callisto_output_asset_sub_path(".map")

			os.system("%s" % config["asset-copy-script-path"])
			os.system("start %s \"%s\"" % (config["engine-exe-path"], map_asset_ref))

		return {'FINISHED'}

#-------------------------------------------------------------------------------------------------
# Class: CallistoClearAssetCacheOperator
#-------------------------------------------------------------------------------------------------
class CallistoClearAssetCacheOperator(bpy.types.Operator):
	bl_label = "Callisto Clear Asset Cache Operator"
	bl_idname = "object.callistoclearassetoperator"
	bl_description = "Clear the Callisto asset cache."
 
	def execute(self, context):
		CallistoAssetCache.clear()
		return {'FINISHED'}


#-------------------------------------------------------------------------------------------------
# Class: CallistoMapExporter
#-------------------------------------------------------------------------------------------------
class CallistoMapExporter():

	def export(self):
		print("********** Exporting Scene As Callisto Map **********")

		config = load_callisto_config()
		if config != None:
			asset_folder_path = config["asset-folder-path"]

			if asset_folder_path == None or asset_folder_path.strip() == "":
				print("Callisto asset folder path has not been set.")
			else:
				world_mesh_asset = CallistoSavableWorldMeshAsset()
				world_mesh_asset.build_from_scene()
				world_mesh_asset.save()

				map_asset = CallistoSavableMapAsset()
				map_asset.build_from_scene(world_mesh_asset)
				map_asset.save()

#-------------------------------------------------------------------------------------------------
# Class: CallistoStaticMeshExporter
#-------------------------------------------------------------------------------------------------
class CallistoStaticMeshExporter():

	def export(self):
		print("********** Exporting Scene As Callisto Static Mesh **********")

		config = load_callisto_config()
		if config != None:
			asset_folder_path = config["asset-folder-path"]

			if asset_folder_path == None or asset_folder_path.strip() == "":
				print("Callisto asset folder path has not been set.")
			else:
				static_mesh_asset = CallistoSavableStaticMeshAsset()
				static_mesh_asset.build_from_scene()
				static_mesh_asset.save()

#-------------------------------------------------------------------------------------------------
# Class: CallistoAssetCache
#-------------------------------------------------------------------------------------------------
class CallistoAssetCache():
	material_assets_by_ref = {}
	actor_assets_by_ref = {}
	static_mesh_assets_by_ref = {}

	@staticmethod
	def clear():
		CallistoAssetCache.material_assets_by_ref = {}
		CallistoAssetCache.actor_assets_by_ref = {}
		CallistoAssetCache.static_mesh_assets_by_ref = {}

	@staticmethod
	def load_material_asset(asset_ref):
		material_asset = None
		
		if asset_ref in CallistoAssetCache.material_assets_by_ref:
			material_asset = CallistoAssetCache.material_assets_by_ref[asset_ref]
		else:
			material_asset = CallistoMaterialAsset()
			material_asset.load_from_asset_ref(asset_ref)
			CallistoAssetCache.material_assets_by_ref[asset_ref] = material_asset

		return material_asset

	@staticmethod
	def load_actor_asset(asset_ref):
		actor_asset = None
		
		if asset_ref in CallistoAssetCache.actor_assets_by_ref:
			actor_asset = CallistoAssetCache.actor_assets_by_ref[asset_ref]
		else:
			actor_asset = CallistoActorAsset()
			actor_asset.load_from_asset_ref(asset_ref)
			CallistoAssetCache.actor_assets_by_ref[asset_ref] = actor_asset

		return actor_asset

	@staticmethod
	def load_static_mesh_asset(asset_ref):
		static_mesh_asset = None
		
		if asset_ref in CallistoAssetCache.static_mesh_assets_by_ref:
			static_mesh_asset = CallistoAssetCache.static_mesh_assets_by_ref[asset_ref]
		else:
			static_mesh_asset = CallistoStaticMeshAsset()
			static_mesh_asset.load_from_asset_ref(asset_ref)
			CallistoAssetCache.static_mesh_assets_by_ref[asset_ref] = static_mesh_asset

		return static_mesh_asset

#-------------------------------------------------------------------------------------------------
# Class: CallistoSavableWorldMeshAsset
#-------------------------------------------------------------------------------------------------
class CallistoSavableWorldMeshAsset():
	def __init__(self):
		self.asset_sub_path = ""
		self.positions = []
		self.normals = []
		self.uvs = []
		self.indecies = []
		self.chunks = []
		self.material_asset_refs = []
		self.pvs_sector_aabbs = []
		self.pvs_asset_ref = ""
		self.grid_origin = None
		self.grid_dimensions = None
		self.grid_cell_size = 1
		self.light_atlases = []
		self.light_atlas_texture_asset_refs = []

	def build_from_scene(self):
		self.asset_sub_path = build_callisto_output_asset_sub_path(".wmesh")

		for obj in bpy.context.scene.objects:
			if obj.callisto_role == "WorldMeshChunk":
				self.add_chunk_from_object(obj)

		for obj in bpy.context.scene.objects:
			if obj.callisto_role == "PvsSector":
				self.add_pvs_sector_aabb_from_object(obj)

		pvs_asset_sub_path = build_callisto_output_asset_sub_path(".pvs")
		self.pvs_asset_ref = "pvs/%s" % pvs_asset_sub_path

	def save(self):
		path = build_callisto_asset_file_path("world-meshes", self.asset_sub_path)

		print("Writing world mesh asset file to '%s'." % path)

		self.calculate_grid_details()

		with open(path, 'w') as f:
			f.write("number-of-verts %d\n" % len(self.positions))
			f.write("number-of-indecies %d\n" % len(self.indecies))
			f.write("number-of-chunks %d\n" % len(self.chunks))
			f.write("number-of-material-asset-refs %d\n" % len(self.material_asset_refs))
			f.write("number-of-light-atlas-texture-asset-refs %d\n" % len(self.light_atlas_texture_asset_refs))
			f.write("number-of-pvs-sector-aabbs %d\n" % len(self.pvs_sector_aabbs))
			f.write("grid-origin %f %f %f\n" % (self.grid_origin.x, self.grid_origin.y, self.grid_origin.z))
			f.write("grid-dimensions %d %d %d\n" % (self.grid_dimensions.x, self.grid_dimensions.y, self.grid_dimensions.z))
			f.write("grid-cell-size %f\n" % self.grid_cell_size)

			f.write("positions\n")
			for position in self.positions:
				f.write("%f %f %f\n" % (position.x, position.y, position.z))

			f.write("normals\n")
			for normal in self.normals:
				f.write("%f %f %f\n" % (normal.x, normal.y, normal.z))

			f.write("material-tex-coords\n")
			for uv in self.uvs:
				#f.write("%f %f\n" % (round(uv.x, 3), round(uv.y, 3)))
				f.write("%f %f\n" % (uv.x, uv.y))

			f.write("indecies\n")
			for index in self.indecies:
				f.write("%d\n" % index)
	
			f.write("chunks\n")
			for chunk in self.chunks:
				f.write("%d %d %d %d %d %d %d\n" % (
					chunk.start_index, chunk.number_of_faces, chunk.material_asset_ref_index,
					chunk.light_island.offset.x, chunk.light_island.offset.y, 
					chunk.light_island.size.x, chunk.light_island.size.y))

			f.write("material-asset-refs\n")
			for material_asset_ref in self.material_asset_refs:
				f.write("%s\n" % material_asset_ref)

			f.write("light-atlas-texture-asset-refs\n")
			for light_atlas_texture_asset_ref in self.light_atlas_texture_asset_refs:
				f.write("%s\n" % light_atlas_texture_asset_ref)

			f.write("pvs-sector-aabbs\n")
			for aabb in self.pvs_sector_aabbs:
				f.write("%f %f %f %f %f %f\n" % 
					(aabb.start.x, aabb.start.y, aabb.start.z,
					aabb.end.x, aabb.end.y, aabb.end.z))

			f.write("pvs-asset-ref\n")
			f.write("%s\n" % self.pvs_asset_ref)
			
	def add_chunk_from_object(self, obj):
		print("Adding chunk from object: " + obj.name + ", chunk number: " + str(len(self.chunks)))

		if obj.callisto_material_asset_sub_path == "":
			print("Object '" + obj.name + "' has no material.")
		else:
			faces = self.build_faces_from_object(obj)
			if faces is not None:
				#print("Built %d faces to add." % len(faces))
				self.add_chunk(faces, obj.callisto_material_asset_sub_path)

	def add_pvs_sector_aabb_from_object(self, obj):
		#print("Adding PVS sector AABB from object: " + obj.name)

		positions = self.build_positions_from_object(obj)
		aabb = CallistoAabb.build_from_positions(positions)
		self.pvs_sector_aabbs.append(aabb)

	def add_chunk(self, faces, material_file_name):
		chunk = CallistoWorldMeshChunk()
		chunk.start_index = len(self.indecies)
		chunk.number_of_faces = len(faces)
		chunk.material_asset_ref_index = self.add_material_asset_ref(material_file_name)
		chunk.light_island = self.calculate_light_island_for_chunk(chunk)
		self.chunks.append(chunk)

		index_lookup = {}

		for face in faces:
			for i in range(0, 3):
				position = face.positions[i]
				normal = face.normals[i]
				uv = face.uvs[i]
				index = -1

				index_key = "%f,%f,%f,%f,%f,%f,%f,%f" % (position.x, position.y, position.z, normal.x, normal.y, normal.z, uv.x, uv.y)
				if index_key in index_lookup:
					index = index_lookup[index_key]
				else:
					index = len(self.positions)
					index_lookup[index_key] = index
					self.positions.append(position)
					self.normals.append(normal)
					self.uvs.append(uv)

				self.indecies.append(index)

	def build_faces_from_object(self, obj):
		faces = None

		bm = bmesh.new()
		bm.from_mesh(obj.data)
		bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)

		uv_layer = bm.loops.layers.uv.active
		if uv_layer is None:
			print("Object '" + obj.name + "' has no UV layer.")
		else:
			faces = []

			for bm_face in bm.faces:
				face = CallistoWorldMeshFace()
				faces.append(face)
				for i in [0, 2, 1]:
					v = obj.matrix_world * bm_face.verts[i].co
					normal = obj.matrix_world.to_3x3() * bm_face.normal#bm_face.verts[i].normal
					uv = bm_face.loops[i][uv_layer].uv
					face.positions.append(CallistoVec3(v.x, v.z, v.y))
					face.normals.append(CallistoVec3(normal.x, normal.z, normal.y))
					face.uvs.append(CallistoVec2(uv.x, 1 - uv.y))

		return faces

	def build_positions_from_object(self, obj):
		positions = []

		bm = bmesh.new()
		bm.from_mesh(obj.data)
		bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)

		for bm_face in bm.faces:
			for i in range(0, 3):
				v = obj.matrix_world * bm_face.verts[i].co
				position = CallistoVec3(v.x, v.z, v.y)
				positions.append(position)

		return positions

	def calculate_light_island_for_chunk(self, chunk):
		light_island = None

		#island_size = CallistoVec2(32 / 4096, 32 / 4096) # FIXME
		island_size = CallistoVec2(64, 64) # FIXME

		for light_atlas in self.light_atlases:
			light_island = self.attempt_to_occupy_island_in_light_atlas(light_atlas, island_size)
			if light_island != None:
				break

		if light_island == None:
			light_atlas = self.push_light_atlas()
			light_island = self.attempt_to_occupy_island_in_light_atlas(light_atlas, island_size)

		if light_island == None:
			raise ValueError("Unable to calculate light island.") 
		
		return light_island

	def push_light_atlas(self):
		print("Pushing new light atlas")
		light_atlas = CallistoLightAtlas(4096)
		self.light_atlases.append(light_atlas)
		self.light_atlas_texture_asset_refs.append("textures/light-atlases/" + build_callisto_output_asset_sub_path("-" + str(len(self.light_atlas_texture_asset_refs) + 1) + ".png"))
		return light_atlas

	def attempt_to_occupy_island_in_light_atlas(self, light_atlas, island_size):
		light_island = None
		for rect in light_atlas.rects:
			if rect.available and rect.rect.size.x >= island_size.x and rect.rect.size.y >= island_size.y:
				light_island = CallistoLightIsland()
				light_island.offset.x = rect.rect.position.x
				light_island.offset.y = rect.rect.position.y
				light_island.size.x = island_size.x
				light_island.size.y = island_size.y
				rect.available = False

				if rect.rect.size.x > light_island.size.x:
					light_atlas.rects.append(CallistoLightAtlasRect(
						light_island.offset.x + light_island.size.x,
						rect.rect.position.y,
						rect.rect.size.x - light_island.size.x,
						rect.rect.size.y))

				if rect.rect.size.y > light_island.size.y:
					light_atlas.rects.append(CallistoLightAtlasRect(
						rect.rect.position.x,
						light_island.offset.y + light_island.size.y,
						light_island.size.x,
						rect.rect.size.y - light_island.size.y))

				break

		return light_island

	def add_material_asset_ref(self, material_asset_sub_path):
		index = -1

		material_asset_ref = "materials/" + material_asset_sub_path

		for i, item in enumerate(self.material_asset_refs):
		    if item == material_asset_ref:
		        index = i
		        break

		if index == -1:
			index = len(self.material_asset_refs)
			self.material_asset_refs.append(material_asset_ref)
			print("Added material " + material_asset_sub_path)

		return index

	def calculate_grid_details(self):
		min = None
		max = None

		for position in self.positions:
			if min == None:
				min = CallistoVec3(position.x, position.y, position.z)
			else:
				if position.x < min.x:
					min.x = position.x
				elif position.y < min.y:
					min.y = position.y
				elif position.z < min.z:
					min.z = position.z

			if max == None:
				max = CallistoVec3(position.x, position.y, position.z)
			else:
				if position.x > max.x:
					max.x = position.x
				elif position.y > max.y:
					max.y = position.y
				elif position.z > max.z:
					max.z = position.z

		min.x = int(math.floor(min.x))
		min.y = int(math.floor(min.y))
		min.z = int(math.floor(min.z))

		max.x = int(math.ceil(max.x))
		max.y = int(math.ceil(max.y))
		max.z = int(math.ceil(max.z))

		self.grid_origin = min

		self.grid_dimensions = CallistoVec3(
			max.x - min.x,
			max.y - min.y,
			max.z - min.z)

#-------------------------------------------------------------------------------------------------
# Class: CallistoSavableStaticMeshAsset
#-------------------------------------------------------------------------------------------------
class CallistoSavableStaticMeshAsset():
	def __init__(self):
		self.asset_sub_path = ""
		self.positions = []
		self.uvs = []
		self.indecies = []
		self.chunks = []
		self.material_asset_refs = []

	def build_from_scene(self):
		self.asset_sub_path = build_callisto_output_asset_sub_path(".smesh")

		for obj in bpy.context.scene.objects:
			if obj.callisto_role == "StaticMeshChunk":
				self.add_chunk_from_object(obj)

	def save(self):
		path = build_callisto_asset_file_path("static-meshes", self.asset_sub_path)

		print("Writing static mesh asset file to '%s'." % path)

		with open(path, 'w') as f:
			f.write("number-of-verts %d\n" % len(self.positions))
			f.write("number-of-indecies %d\n" % len(self.indecies))
			f.write("number-of-chunks %d\n" % len(self.chunks))
			f.write("number-of-material-asset-refs %d\n" % len(self.material_asset_refs))

			f.write("positions\n")
			for position in self.positions:
				f.write("%f %f %f\n" % (position.x, position.y, position.z))

			f.write("material-tex-coords\n")
			for uv in self.uvs:
				f.write("%f %f\n" % (uv.x, uv.y))

			f.write("indecies\n")
			for index in self.indecies:
				f.write("%d\n" % index)
	
			f.write("chunks\n")
			for chunk in self.chunks:
				f.write("%d %d %d\n" % (chunk.start_index, chunk.number_of_faces, chunk.material_asset_ref_index))

			f.write("material-asset-refs\n")
			for material_asset_ref in self.material_asset_refs:
				f.write("%s\n" % material_asset_ref)

	def add_chunk_from_object(self, obj):
		print("Adding chunk from object: " + obj.name)

		if obj.callisto_material_asset_sub_path == "":
			print("Object '" + obj.name + "' has no material.")
		else:
			faces = self.build_faces_from_object(obj)
			if faces is not None:
				print("Built %d faces to add." % len(faces))
				self.add_chunk(faces, obj.callisto_material_asset_sub_path)

	def add_chunk(self, faces, material_file_name):
		chunk = CallistoStaticMeshChunk()
		chunk.start_index = len(self.indecies)
		chunk.number_of_faces = len(faces)
		chunk.material_asset_ref_index = self.add_material_asset_ref(material_file_name)
		self.chunks.append(chunk)

		index_lookup = {}

		for face in faces:
			for i in range(0, 3):
				position = face.positions[i]
				uv = face.uvs[i]
				index = -1

				index_key = "%f,%f,%f,%f,%f" % (position.x, position.y, position.z, uv.x, uv.y)
				if index_key in index_lookup:
					index = index_lookup[index_key]
				else:
					index = len(self.positions)
					index_lookup[index_key] = index
					self.positions.append(position)
					self.uvs.append(uv)

				self.indecies.append(index)

	def build_faces_from_object(self, obj):
		faces = None

		bm = bmesh.new()
		bm.from_mesh(obj.data)
		bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)

		uv_layer = bm.loops.layers.uv.active
		if uv_layer is None:
			print("Object '" + obj.name + "' has no UV layer.")
		else:
			faces = []

			for bm_face in bm.faces:
				face = CallistoStaticMeshFace()
				faces.append(face)
				for i in [0, 2, 1]:
					v = obj.matrix_world * bm_face.verts[i].co
					uv = bm_face.loops[i][uv_layer].uv
					face.positions.append(CallistoVec3(v.x, v.z, v.y))
					face.uvs.append(CallistoVec2(uv.x, uv.y))

		return faces

	def build_positions_from_object(self, obj):
		positions = []

		bm = bmesh.new()
		bm.from_mesh(obj.data)
		bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)

		for bm_face in bm.faces:
			for i in range(0, 3):
				v = obj.matrix_world * bm_face.verts[i].co
				position = CallistoVec3(v.x, v.z, v.y)
				positions.append(position)

		return positions

	def add_material_asset_ref(self, material_asset_sub_path):
		index = -1

		material_asset_ref = "materials/" + material_asset_sub_path

		for i, item in enumerate(self.material_asset_refs):
		    if item == material_asset_ref:
		        index = i
		        break

		if index == -1:
			index = len(self.material_asset_refs)
			self.material_asset_refs.append(material_asset_ref)
			print("Added material " + material_asset_sub_path)

		return index

#-------------------------------------------------------------------------------------------------
# Class: CallistoSavableMapAsset
#-------------------------------------------------------------------------------------------------
class CallistoSavableMapAsset():
	def __init__(self):
		self.asset_sub_path = ""
		self.world_mesh_asset_refs = []
		self.pvs_asset_refs = []
		self.static_mesh_asset_refs = []
		self.material_asset_refs = []
		self.texture_asset_refs = []
		self.actor_asset_refs = []
		self.sprite_sheet_asset_refs = []
		self.world_mesh_asset_ref = []
		self.actor_instances = []

	def build_from_scene(self, worldMeshAsset):
		self.asset_sub_path = build_callisto_output_asset_sub_path(".map")

		self.world_mesh_asset_ref = "world-meshes/%s" % worldMeshAsset.asset_sub_path
		self.world_mesh_asset_refs.append(self.world_mesh_asset_ref)
		self.pvs_asset_refs.append(worldMeshAsset.pvs_asset_ref)

		for material_asset_ref in worldMeshAsset.material_asset_refs:
			self.include_material_asset(material_asset_ref)

		for texture_asset_ref in worldMeshAsset.light_atlas_texture_asset_refs:
			self.include_texture_asset(texture_asset_ref)

		for obj in bpy.context.scene.objects:
			if obj.callisto_role == "ActorInstance":
				self.add_actor_instance_from_object(obj)

	def add_actor_instance_from_object(self, obj):
		actor_asset_ref = "actors/" + obj.callisto_actor_asset_sub_path
		self.include_actor_asset(actor_asset_ref)

		actor_instance = CallistoActorInstance()
		actor_instance.name = obj.name
		actor_instance.asset_ref = actor_asset_ref

		position = obj.matrix_world * obj.location
		position = obj.location
		actor_instance.position = CallistoVec3(position.x, position.z, position.y)
		actor_instance.rotation = CallistoVec3(obj.rotation_euler.x, -obj.rotation_euler.z, obj.rotation_euler.y)
		actor_instance.config_json = build_callisto_actor_instance_config_json_from_object(obj)

		self.actor_instances.append(actor_instance)

	def include_material_asset(self, asset_ref):
		if asset_ref not in self.material_asset_refs:
			self.material_asset_refs.append(asset_ref)

			material_asset = CallistoAssetCache.load_material_asset(asset_ref)
			self.include_texture_asset(material_asset.diffuse_texture_asset_ref)

	def include_actor_asset(self, asset_ref):
		if asset_ref not in self.actor_asset_refs:
			self.actor_asset_refs.append(asset_ref)
			
			actor_asset = CallistoAssetCache.load_actor_asset(asset_ref)
			for other_asset_ref in actor_asset.static_mesh_asset_refs:
				self.include_static_mesh_asset(other_asset_ref)
			
			for other_asset_ref in actor_asset.sprite_sheet_asset_refs:
				self.include_sprite_sheet_mesh_asset(other_asset_ref)
			
			for other_asset_ref in actor_asset.texture_asset_refs:
				self.include_texture_asset(other_asset_ref)

			for other_asset_ref in actor_asset.dependency_actor_asset_refs:
				self.include_actor_asset(other_asset_ref)
			
	def include_static_mesh_asset(self, asset_ref):
		if asset_ref not in self.static_mesh_asset_refs:
			self.static_mesh_asset_refs.append(asset_ref)

			static_mesh_asset = CallistoAssetCache.load_static_mesh_asset(asset_ref)
			for other_asset_ref in static_mesh_asset.material_asset_refs:
				self.include_material_asset(other_asset_ref)

	def include_sprite_sheet_mesh_asset(self, asset_ref):
		if asset_ref not in self.sprite_sheet_asset_refs:
			self.sprite_sheet_asset_refs.append(asset_ref)

	def include_texture_asset(self, asset_ref):
		if asset_ref not in self.texture_asset_refs:
			self.texture_asset_refs.append(asset_ref)

	def save(self):
		path = build_callisto_asset_file_path("maps", self.asset_sub_path)

		print("Writing map asset file to '%s'." % path)

		with open(path, 'w') as f:
			f.write("{\n")
			f.write("\t\"world-mesh-asset-refs\": [%s],\n" % self.format_json_array(self.world_mesh_asset_refs))
			f.write("\t\"pvs-asset-refs\": [%s],\n" % self.format_json_array(self.pvs_asset_refs))
			f.write("\t\"static-mesh-asset-refs\": [%s],\n" % self.format_json_array(self.static_mesh_asset_refs))
			f.write("\t\"material-asset-refs\": [%s],\n" % self.format_json_array(self.material_asset_refs))
			f.write("\t\"texture-asset-refs\": [%s],\n" % self.format_json_array(self.texture_asset_refs))
			f.write("\t\"actor-asset-refs\": [%s],\n" % self.format_json_array(self.actor_asset_refs))
			f.write("\t\"sprite-sheet-asset-refs\": [%s],\n" % self.format_json_array(self.sprite_sheet_asset_refs))
			f.write("\t\"world-mesh-asset-ref\": \"%s\",\n" % self.world_mesh_asset_ref)

			f.write("\t\"actor-instances\": [\n")
			#f.write("{\"name\": \"Player\", \"asset-ref\": \"actors/first-person-player.actor\", \"position\": [2.5, 0.3, 2.5], \"rotation\": [0.0, 0.0, 0.0], \"config\": { \"auto-set-as-default-camera\": true } }")
			
			seperator = ""
			for actor_instance in self.actor_instances:
				f.write(seperator)
				f.write("\t\t{\n\t\t\t\"name\": \"%s\",\n\t\t\t\"asset-ref\": \"%s\",\n\t\t\t\"position\": [%f, %f, %f],\n\t\t\t\"rotation\": [%f, %f, %f],\n\t\t\t\"config\": %s\t\t}" % 
					(actor_instance.name, actor_instance.asset_ref, 
					actor_instance.position.x, actor_instance.position.y, actor_instance.position.z, 
					actor_instance.rotation.x, actor_instance.rotation.y, actor_instance.rotation.z,
					actor_instance.config_json))
				seperator = ",\n"

			f.write("\n\t]\n")

			f.write("}")

	def format_json_array(self, array):
		return ", ".join(["\"" + str(item) + "\"" for item in array])
	
#-------------------------------------------------------------------------------------------------
# Class: CallistoActorInstance
#-------------------------------------------------------------------------------------------------	
class CallistoActorInstance():
	def __init__(self):
		self.name = ""
		self.asset_ref = ""
		self.position = CallistoVec3(0, 0, 0)
		self.rotation = CallistoVec3(0, 0, 0)
		self.config_json = ""

#-------------------------------------------------------------------------------------------------
# Class: CallistoMaterialAsset
#-------------------------------------------------------------------------------------------------
class CallistoMaterialAsset():
	def __init__(self):
		self.asset_sub_path = ""
		self.diffuse_texture_asset_ref = ""

	def load_from_asset_ref(self, asset_ref):
		self.asset_sub_path = asset_ref.replace("materials/", "")

		data = load_callisto_asset_json("materials", self.asset_sub_path)
		self.diffuse_texture_asset_ref = data["diffuse-texture-asset-ref"]

		print("Loaded material asset '%s'." % asset_ref)

#-------------------------------------------------------------------------------------------------
# Class: CallistoActorAsset
#-------------------------------------------------------------------------------------------------
class CallistoActorAsset():
	def __init__(self):
		self.asset_sub_path = ""
		self.static_mesh_asset_refs = []
		self.sprite_sheet_asset_refs = []
		self.texture_asset_refs = []
		self.dependency_actor_asset_refs = []

	def load_from_asset_ref(self, asset_ref):
		self.asset_sub_path = asset_ref.replace("actors/", "")

		data = load_callisto_asset_json("actors", self.asset_sub_path)
		self.static_mesh_asset_refs = data.get("static-mesh-asset-refs", [])
		self.sprite_sheet_asset_refs = data.get("sprite-sheet-asset-refs", [])
		self.texture_asset_refs = data.get("texture-asset-refs", [])
		self.dependency_actor_asset_refs = data.get("dependency-actor-asset-refs", [])

		print("Loaded actor asset '%s'." % asset_ref)

#-------------------------------------------------------------------------------------------------
# Class: CallistoStaticMeshAsset
#-------------------------------------------------------------------------------------------------
class CallistoStaticMeshAsset():
	def __init__(self):
		self.asset_sub_path = ""
		self.material_asset_refs = []

	def load_from_asset_ref(self, asset_ref):
		self.asset_sub_path = asset_ref.replace("static-meshes/", "")

		path = build_callisto_asset_file_path("static-meshes", self.asset_sub_path)
		file = open(path)
		number_of_material_asset_refs = 0
		is_reading_material_asset_refs = False

		while True:
			line = file.readline()
			if line == "":
				break
			elif line.startswith("number-of-material-asset-refs"):
				number_of_material_asset_refs = int(line.split(" ")[1])
			elif line.strip() == "material-asset-refs":
				is_reading_material_asset_refs = True
			elif is_reading_material_asset_refs:
				self.material_asset_refs.append(line.strip())

		print("Loaded static mesh asset '%s'." % asset_ref)

#-------------------------------------------------------------------------------------------------
# Class: CallistoVec3
#-------------------------------------------------------------------------------------------------
class CallistoVec3():
	def __init__(self, x, y, z):
		self.x = x
		self.y = y
		self.z = z

	@staticmethod
	def copy(dest, src):
		dest.x = src.x
		dest.y = src.y
		dest.z = src.z

#-------------------------------------------------------------------------------------------------
# Class: CallistoVec2
#-------------------------------------------------------------------------------------------------
class CallistoVec2():
	def __init__(self, x, y):
		self.x = x
		self.y = y

#-------------------------------------------------------------------------------------------------
# Class: CallistoRect
#-------------------------------------------------------------------------------------------------
class CallistoRect():
	def __init__(self, x, y, width, height):
		self.position = CallistoVec2(x, y)
		self.size = CallistoVec2(width, height)

#-------------------------------------------------------------------------------------------------
# Class: CallistoAabb
#-------------------------------------------------------------------------------------------------
class CallistoAabb():
	def __init__(self, start, end):
		self.start = CallistoVec3(start.x, start.y, start.z)
		self.end = CallistoVec3(end.x, end.y, end.z)

	@staticmethod
	def build_from_positions(positions):
		start = CallistoVec3(0, 0, 0)
		end = CallistoVec3(0, 0, 0)
		initialised = False

		for position in positions:
			if not initialised:
				CallistoVec3.copy(start, position)
				CallistoVec3.copy(end, position)
				initialised = True
			else:
				if position.x < start.x:
					start.x = position.x
				if position.y < start.y:
					start.y = position.y
				if position.z < start.z:
					start.z = position.z
				if position.x > end.x:
					end.x = position.x
				if position.y > end.y:
					end.y = position.y
				if position.z > end.z:
					end.z = position.z

		return CallistoAabb(start, end)

#-------------------------------------------------------------------------------------------------
# Class: CallistoWorldMeshFace
#-------------------------------------------------------------------------------------------------
class CallistoWorldMeshFace():
	def __init__(self):
		self.positions = []
		self.normals = []
		self.uvs = []

#-------------------------------------------------------------------------------------------------
# Class: CallistoStaticMeshFace
#-------------------------------------------------------------------------------------------------
class CallistoStaticMeshFace():
	def __init__(self):
		self.positions = []
		self.uvs = []

#-------------------------------------------------------------------------------------------------
# Class: CallistoWorldMeshChunk
#-------------------------------------------------------------------------------------------------
class CallistoWorldMeshChunk():
	def __init__(self):
		self.start_index = 0
		self.number_of_faces = 0
		self.material_asset_ref_index = 0
		self.light_island = CallistoLightIsland()

#-------------------------------------------------------------------------------------------------
# Class: CallistoStaticMeshChunk
#-------------------------------------------------------------------------------------------------
class CallistoStaticMeshChunk():
	def __init__(self):
		self.start_index = 0
		self.number_of_faces = 0
		self.material_asset_ref_index = 0

#-------------------------------------------------------------------------------------------------
# Class: CallistoLightAtlas
#-------------------------------------------------------------------------------------------------
class CallistoLightAtlas():
	def __init__(self, size):
		self.size = size
		self.rects = []
		self.rects.append(CallistoLightAtlasRect(0, 0, size, size))

#-------------------------------------------------------------------------------------------------
# Class: CallistoLightAtlasRect
#-------------------------------------------------------------------------------------------------
class CallistoLightAtlasRect():
	def __init__(self, x, y, width, height):
		self.rect = CallistoRect(x, y, width, height)
		self.available = True

#-------------------------------------------------------------------------------------------------
# Class: CallistoLightIsland
#-------------------------------------------------------------------------------------------------
class CallistoLightIsland():
	def __init__(self):
		self.offset = CallistoVec2(0, 0)
		self.size = CallistoVec2(0, 0)

#-------------------------------------------------------------------------------------------------
# Function: build_callisto_asset_file_path
#-------------------------------------------------------------------------------------------------
def build_callisto_asset_file_path(asset_folder_name, asset_sub_path):
	config = load_callisto_config()
	asset_folder_path = config["asset-folder-path"]
	return "%s/%s/%s" % (asset_folder_path, asset_folder_name, asset_sub_path)

#-------------------------------------------------------------------------------------------------
# Function: load_callisto_asset_json
#-------------------------------------------------------------------------------------------------
def load_callisto_asset_json(asset_folder_name, asset_sub_path):
	path = build_callisto_asset_file_path(asset_folder_name, asset_sub_path)
	json_string = open(path).read()
	return json.loads(json_string)

#-------------------------------------------------------------------------------------------------
# Function: build_callisto_output_asset_sub_path
#-------------------------------------------------------------------------------------------------
def build_callisto_output_asset_sub_path(file_extension):
	base_file_name = bpy.path.basename(bpy.data.filepath)
	output_folder_name = bpy.context.scene.callisto_output_folder_name

	asset_sub_path = base_file_name.replace(".blend", file_extension)
	if output_folder_name is not None and output_folder_name != "":
		asset_sub_path = output_folder_name + "/" + asset_sub_path

	return asset_sub_path

#-------------------------------------------------------------------------------------------------
# Function: load_callisto_config
#-------------------------------------------------------------------------------------------------
def load_callisto_config():
	config = None
	config_file_path = bpy.context.scene.callisto_config_file_path
	if config_file_path == None or config_file_path == "":
		print("Callisto config file path not set")
	else:
		json_string = open(config_file_path).read()
		config = json.loads(json_string)
		
	return config

#-------------------------------------------------------------------------------------------------
# Function: evaluate_updated_callisto_objects
#-------------------------------------------------------------------------------------------------
@persistent
def evaluate_updated_callisto_objects(scene):
	#updated_objects = []
	for obj in scene.objects:
		if obj.is_updated:
			for pair in CallistoConfig.scene_object_enum_string_pairs:
				value = getattr(obj, pair[0])
				setattr(obj, pair[1], value)

#-------------------------------------------------------------------------------------------------
# Function: init_callisto_objects
#-------------------------------------------------------------------------------------------------
@persistent
def init_callisto_objects(dummy):
	#print("Init Callisto objects")
	for obj in bpy.context.scene.objects:
		for pair in CallistoConfig.scene_object_enum_string_pairs:
			value = getattr(obj, pair[1])
			if value != None and value != "":
				setattr(obj, pair[0], value)

#-------------------------------------------------------------------------------------------------
# Function: register
#-------------------------------------------------------------------------------------------------
def register():
	print("Registering Callisto Tools")

	roles = [
		("None", "None", ""), 
		("WorldMeshChunk", "World Mesh Chunk", ""), 
		("StaticMeshChunk", "Static Mesh Chunk", ""), 
		("ActorInstance", "Actor Instance", ""), 
		("PvsSector", "PVS Sector", "")]

	CallistoConfig.scene_object_enum_string_pairs.extend(
		[
			("callisto_material_asset_enum", "callisto_material_asset_sub_path"),
			("callisto_actor_asset_enum", "callisto_actor_asset_sub_path")
		])

	availableMaterialAssets = load_callisto_available_material_assets_enum_items()
	availableActorAssets = load_callisto_available_actor_assets_enum_items()

	# Register scene properties.
	bpy.types.Scene.callisto_config_file_path = bpy.props.StringProperty(name="Config File Path")
	#bpy.types.Scene.callisto_asset_folder_path = bpy.props.StringProperty(name="Asset Folder Path")
	bpy.types.Scene.callisto_output_folder_name = bpy.props.StringProperty(name="Output Folder Name")
	#bpy.types.Scene.callisto_asset_copy_script_path = bpy.props.StringProperty(name="Asset Copy Script Path")
	#bpy.types.Scene.callisto_pvs_generator_exe_path = bpy.props.StringProperty(name="PVS Generator EXE Path")
	#bpy.types.Scene.callisto_engine_exe_path = bpy.props.StringProperty(name="Engine EXE Path")
	#bpy.types.Scene.callisto_grid_dimensions = bpy.props.IntVectorProperty(name="Grid Dimensions", size=3)
	#bpy.types.Scene.callisto_grid_cell_size = bpy.props.FloatProperty(name="Grid Cell Size")

	# Register general object properties.
	bpy.types.Object.callisto_role = bpy.props.EnumProperty(name="Role", items=roles)
	bpy.types.Object.callisto_material_asset_enum = bpy.props.EnumProperty(name="Material Asset", items=availableMaterialAssets)
	bpy.types.Object.callisto_material_asset_sub_path = bpy.props.StringProperty(name="Material Asset Sub Path")
	bpy.types.Object.callisto_actor_asset_enum = bpy.props.EnumProperty(name="Actor Asset", items=availableActorAssets)
	bpy.types.Object.callisto_actor_asset_sub_path = bpy.props.StringProperty(name="Actor Asset Sub Path")
	
	# Register actor instance object properties.
	register_callisto_actor_instance_object_properties()
		
	# Register operators.
	bpy.utils.register_class(CallistoExportMapOperator)
	bpy.utils.register_class(CallistoExportStaticMeshOperator)
	bpy.utils.register_class(CallistoGeneratePvsOperator)
	bpy.utils.register_class(CallistoPlayMapOperator)
	bpy.utils.register_class(CallistoClearAssetCacheOperator)

	# Register panels.
	bpy.utils.register_class(CallistoScenePropertiesPanel)
	bpy.utils.register_class(CallistoObjectPropertiesPanel)

	# Register global event handlers.
	bpy.app.handlers.load_post.append(init_callisto_objects)
	bpy.app.handlers.scene_update_post.append(evaluate_updated_callisto_objects)

#-------------------------------------------------------------------------------------------------
# Function: unregister
#-------------------------------------------------------------------------------------------------
def unregister():
	print("Unregistering Callisto Tools")

	# unregister operators.
	bpy.utils.unregister_class(CallistoExportMapOperator)
	bpy.utils.unregister_class(CallistoExportStaticMeshOperator)
	bpy.utils.unregister_class(CallistoGeneratePvsOperator)
	bpy.utils.unregister_class(CallistoPlayMapOperator)
	bpy.utils.unregister_class(CallistoClearAssetCacheOperator)

	# Unregister panels.
	bpy.utils.unregister_class(CallistoScenePropertiesPanel)
	bpy.utils.unregister_class(CallistoObjectPropertiesPanel)

###############################################################################################
####################################### JSON Functions ########################################
###############################################################################################

def format_delimited_string_as_json_string_array(s, delimiter):
	json = ""
	parts = s.split(delimiter)
	seperator = ""
	for part in parts:
		json += seperator + "\"" + part.strip() + "\""
		seperator = ", "
	json = "[" + json + "]"

	return json

###############################################################################################
################################### Game-Specific Functions ###################################
###############################################################################################

#-------------------------------------------------------------------------------------------------
# Function: load_callisto_available_material_assets_enum_items
#-------------------------------------------------------------------------------------------------
def load_callisto_available_material_assets_enum_items():
	return [
		("debug/floor-1.mat", "Debug/Floor 1", ""),
		("debug/floor-2.mat", "Debug/Floor 2", ""),
		("debug/wall-1.mat", "Debug/Wall 1", ""),
		("debug/wall-2.mat", "Debug/Wall 2", "")
	]

#-------------------------------------------------------------------------------------------------
# Function: load_callisto_available_actor_assets_enum_items
#-------------------------------------------------------------------------------------------------
def load_callisto_available_actor_assets_enum_items():
	return [
		("", "-- Choose --", ""),
		("first-person-player.actor", "First Person Player", ""),
		("robot.actor", "Robot", ""),
		("door.actor", "Door", ""),
		("key.actor", "Key", ""),
		("spawn-point.actor", "Spawn Point", ""),
		("spawn-trigger.actor", "Spawn Trigger", "")
	]

#-------------------------------------------------------------------------------------------------
# Function: register_callisto_actor_instance_object_properties
#-------------------------------------------------------------------------------------------------
def register_callisto_actor_instance_object_properties():
	availableActorAssets = load_callisto_available_actor_assets_enum_items()

	availableKeyTypes = [
		("none", "None", ""),
		("red", "Red", ""),
		("blue", "Blue", ""),
		("green", "Green", "")
	]

	# First Person Player:
	bpy.types.Object.callisto_actor_instance_auto_set_as_default_camera = bpy.props.BoolProperty(name="Auto Set As Default Camera") 
	
	# Spawn Point:
	bpy.types.Object.callisto_actor_instance_spawnee_actor_asset_enum = bpy.props.EnumProperty(name="Spawnee Actor Asset", items=availableActorAssets)
	bpy.types.Object.callisto_actor_instance_spawnee_actor_asset_sub_path = bpy.props.StringProperty(name="Spawnee Actor Asset Sub Path")

	# Spawn Trigger:
	bpy.types.Object.callisto_actor_instance_spawn_points = bpy.props.StringProperty(name="Spawn Points")
	bpy.types.Object.callisto_actor_instance_reward_drop_actor_asset_enum = bpy.props.EnumProperty(name="Reward Drop Actor Asset", items=availableActorAssets)
	bpy.types.Object.callisto_actor_instance_reward_drop_actor_asset_sub_path = bpy.props.StringProperty(name="Reward Drop Actor Asset Sub Path")
	bpy.types.Object.callisto_actor_instance_reward_drop_actor_config_key_type = bpy.props.EnumProperty(name="Reward Drop Key Type", items=availableKeyTypes, default="none")

	# Door:
	bpy.types.Object.callisto_actor_instance_is_locked = bpy.props.BoolProperty(name="Locked")
	bpy.types.Object.callisto_actor_instance_required_key_type = bpy.props.EnumProperty(name="Required Key Type", items=availableKeyTypes, default="none")

	CallistoConfig.scene_object_enum_string_pairs.extend(
		[
			("callisto_actor_instance_spawnee_actor_asset_enum", "callisto_actor_instance_spawnee_actor_asset_sub_path"),
			("callisto_actor_instance_reward_drop_actor_asset_enum", "callisto_actor_instance_reward_drop_actor_asset_sub_path")
		])

#-------------------------------------------------------------------------------------------------
# Function: setup_callisto_actor_instance_object_properties_panel
#-------------------------------------------------------------------------------------------------
def setup_callisto_actor_instance_object_properties_panel(layout, obj):
	if obj.callisto_actor_asset_sub_path == "first-person-player.actor":
		layout.prop(obj, "callisto_actor_instance_auto_set_as_default_camera")

	elif obj.callisto_actor_asset_sub_path == "spawn-point.actor":
		layout.prop(obj, "callisto_actor_instance_spawnee_actor_asset_enum")
		#layout.prop(obj, "callisto_actor_instance_spawnee_actor_asset_sub_path")

	elif obj.callisto_actor_asset_sub_path == "spawn-trigger.actor":
		layout.prop(obj, "callisto_actor_instance_spawn_points")
		layout.prop(obj, "callisto_actor_instance_reward_drop_actor_asset_enum")
		#layout.prop(obj, "callisto_actor_instance_reward_drop_actor_asset_sub_path")

		if obj.callisto_actor_instance_reward_drop_actor_asset_sub_path == "key.actor":
			layout.prop(obj, "callisto_actor_instance_reward_drop_actor_config_key_type")

	elif obj.callisto_actor_asset_sub_path == "door.actor":
		layout.prop(obj, "callisto_actor_instance_is_locked")
		layout.prop(obj, "callisto_actor_instance_required_key_type")

#-------------------------------------------------------------------------------------------------
# Function: build_callisto_actor_instance_config_json_from_object
#-------------------------------------------------------------------------------------------------
def build_callisto_actor_instance_config_json_from_object(obj):
	config_json = "\t{\n"

	if obj.callisto_actor_asset_sub_path == "first-person-player.actor":
		config_json += "\t\t\t\t\"auto-set-as-default-camera\": %s\n" % str(obj.callisto_actor_instance_auto_set_as_default_camera).lower()

	elif obj.callisto_actor_asset_sub_path == "spawn-point.actor":
		config_json += "\t\t\t\t\"spawnee-actor-asset-ref\": \"%s\"\n" % ("actors/" + obj.callisto_actor_instance_spawnee_actor_asset_sub_path)
		
	elif obj.callisto_actor_asset_sub_path == "spawn-trigger.actor":
		config_json += "\t\t\t\t\"spawn-points\": %s,\n" % (format_delimited_string_as_json_string_array(obj.callisto_actor_instance_spawn_points, ","))
		config_json += "\t\t\t\t\"reward-drop-actor-asset-ref\": \"%s\",\n" % ("actors/" + obj.callisto_actor_instance_reward_drop_actor_asset_sub_path)

		if obj.callisto_actor_instance_reward_drop_actor_asset_sub_path == "key.actor":
			config_json += "\t\t\t\t\"reward-drop-actor-config\": {\n"
			config_json += "\t\t\t\t\t\"key-type\": \"%s\"\n" % (obj.callisto_actor_instance_reward_drop_actor_config_key_type)
			config_json += "\t\t\t\t}\n"
			
	elif obj.callisto_actor_asset_sub_path == "door.actor":
		config_json += "\t\t\t\t\"is-locked\": %s,\n" % str(obj.callisto_actor_instance_is_locked).lower()
		config_json += "\t\t\t\t\"required-key-type\": \"%s\"\n" % (obj.callisto_actor_instance_required_key_type)
		
	config_json += "\t\t\t}\n"
	return config_json