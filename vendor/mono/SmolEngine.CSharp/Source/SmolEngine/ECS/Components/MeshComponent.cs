using System;
using System.Runtime.CompilerServices;

namespace SmolEngine
{
    struct MeshComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static bool LoadModel_EX(string path, uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint LoadMaterial_EX(string path, uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static bool SetMaterial_EX(uint mesh_index, uint material_id, uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void SetVisible_EX(uint entity_id, bool value); 

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void ResetAll_EX(uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint GetChildsCount_EX(uint entity_id);


        private bool _IsVisible;
        private bool _IsActive;
        private uint _Handler;

        public bool IsVisible
        {
            get { return _IsVisible; }
            set { _IsVisible = value; SetVisible_EX(_Handler, value); }
        }

        public bool LoadModel(string path)
        {
            return LoadModel_EX(path, _Handler);
        }

        public bool IsActibe()
        {
            return _IsActive;
        }

        public uint LoadMaterial(string path)
        {
            return LoadMaterial_EX(path, _Handler);
        }

        public bool SetMaterial(uint index, uint material_id)
        {
            return SetMaterial_EX(index, material_id, _Handler);
        }

        // sub meshes
        public uint GetChildsCount()
        {
            return GetChildsCount_EX(_Handler);
        }

        public void ResetAll()
        {
            ResetAll_EX(_Handler);
        }
    }
}
