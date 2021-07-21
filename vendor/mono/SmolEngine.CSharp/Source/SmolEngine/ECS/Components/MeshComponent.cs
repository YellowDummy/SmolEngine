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


        private bool                    _IsVisible;
        private bool                    _IsActive;
        private readonly unsafe uint*   _Handler;

        public bool IsVisible
        {
            get { return _IsVisible; }
            set { _IsVisible = value; OnValueChanged(); }
        }

        public bool LoadModel(string path)
        {
            unsafe
            {
                if (_Handler != null)
                    return LoadModel_EX(path, *_Handler);
            }

            return false;
        }

        public uint LoadMaterial(string path)
        {
            unsafe
            {
                if (_Handler != null)
                {
                    _IsActive = true;
                    return LoadMaterial_EX(path, *_Handler);
                }
            }

            return 0;
        }

        public bool SetMaterial(uint index, string material_path)
        {
            unsafe
            {
                if (_Handler != null)
                    return SetMaterial_EX(index, material_path, *_Handler);
            }

            return false;
        }

        public bool IsActibe()
        {
            return _IsActive;
        }

        // sub meshes
        public uint GetChildsCount()
        {
            unsafe
            {
                if (_Handler != null)
                    return GetChildsCount_EX(*_Handler);
            }

            return 0;
        }

        public void ResetAll()
        {
            unsafe
            {
                if (_Handler != null)
                    ResetAll_EX(*_Handler);
            }
        }

        private void OnValueChanged()
        {
            unsafe
            {
                if (_Handler != null)
                    Utils.OnComponentUpdated(ref this, *_Handler);
            }
        }
    }
}
