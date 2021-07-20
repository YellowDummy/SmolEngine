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
        extern static bool SetMaterial_EX(uint mesh_index, string material_path, uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static void ResetAll_EX(uint entity_id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static uint GetChildsCount_EX(uint entity_id);


        private bool _IsVisible;
        private readonly bool _IsActive;
        private readonly uint _Handler;

        public bool IsVisible
        {
            get { return _IsVisible; }
            set { _IsVisible = value; OnChange(); }
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

        public bool SetMaterial(uint index, string material_path)
        {
            return SetMaterial_EX(index, material_path, _Handler);
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

        private void OnChange()
        {
            var copy = this;
            unsafe
            {
                bool result = Actor.SetComponent_EX(&copy, _Handler, (ushort)ComponentTypeEX.Mesh);
                if (result)
                {
                    this = copy;
                }
            }
        }
    }
}
