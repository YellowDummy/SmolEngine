using System;
using SmolEngine;

namespace SmolEngine
{
    public class Tests
    {
        Tests()
        {
            Console.WriteLine("Test init");
        }

        private void CallMe(uint val1, uint val2)
        {
            Console.WriteLine($"Value1: {val1}, Value2: {val2}");
        }
    }
}

public class CameraBehaviour: BehaviourPrimitive
{
    TransformComponent tranform;
    CameraComponent camera;

    public int Health = 100;
    public float Speed = 2.0f;
    public string AssetPath = "../venod/";

    private void OnBegin()
    {
        var t_result = GetComponent<TransformComponent>();
        if (t_result.HasValue) { tranform = t_result.Value; }

        var init_result = AddComponent<CameraComponent>();
        if(init_result.HasValue) { camera = init_result.Value; }

        camera.IsPrimary = true;
        tranform.Position = new Vector3(0, 25, 0);
    }

    ~CameraBehaviour()
    {
        SLog.WriteLine("Destoyed!");
    }

    private void OnUpdate() {  }
    private void OnDestroy() { }
}

public class GameLogic : BehaviourPrimitive
{
    private void OnBegin()
    {
        SLog.WriteLine("My name: " + GetName());
        SLog.WriteLine("My tag: " + GetTag());

        var t_result = GetComponent<TransformComponent>();
        if(t_result.HasValue)
        {
            TransformComponent tranform = t_result.Value;
            tranform.Position = new Vector3(0, 2, 0);
        }

        var m_result = AddComponent < MeshComponent>();
        if(m_result.HasValue)
        {
            MeshComponent mesh = m_result.Value;
            mesh.LoadModel("../samples/models/tank.gltf");
        }

        var r_result = AddComponent<RigidBodyComponent>();
        if(r_result.HasValue)
        {
            var rbInfo = new RigidBodyCreateInfo();
            RigidBodyComponent rb = r_result.Value;

            rbInfo.CreateDefault();
            rbInfo.eType = RigidBodyType.Dynamic;

            rb.Create(rbInfo);
            rb.AddImpulse(new Vector3(0, 20, 0));
        }
    }

    private void OnUpdate()
    {
        if (Input.IsKeyPressed(KeyCode.V))
        {
            SLog.WriteLine($"Pressed V Key!, actor ID is {GetID()}", LogLevel.Error);
        }
    }

    private void OnDestroy()
    {

    }

    // Optional
    private void OnCollisionBegin(uint another_id, bool is_trigger)
    {

    }

    private void OnCollisionEnd(uint another_id, bool is_trigger)
    {

    }
}