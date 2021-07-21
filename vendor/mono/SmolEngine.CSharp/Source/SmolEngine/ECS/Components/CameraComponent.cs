using System;
using System.Runtime.InteropServices;

namespace SmolEngine
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CameraComponent
    {
        private float                 _FOV;
        private float                 _zNear;
        private float                 _zFar;
        private float                 _Zoom;
        private bool                  _IsPrimary;
        private readonly unsafe uint* _Handler;

        public bool IsPrimary
        {
            get { return _IsPrimary; }
            set { _IsPrimary = value; OnValueChanged(); }
        }

        public float FOV
        {
            get { return _FOV; }
            set { _FOV = value; OnValueChanged();  }
        }

        public float NearClip
        {
            get { return _zNear; }
            set { _zNear = value; OnValueChanged(); }
        }

        public float FarClip
        {
            get { return _zFar; }
            set { _zFar = value; OnValueChanged(); }
        }

        public float Zoom
        {
            get { return _Zoom; }
            set { _Zoom = value; OnValueChanged(); }
        }

        private void OnValueChanged()
        {
            unsafe
            {
                if(_Handler != null)
                    Utils.OnComponentUpdated(ref this, *_Handler);
            }
        }


    }
}
