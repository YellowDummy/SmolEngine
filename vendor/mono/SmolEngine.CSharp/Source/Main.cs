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

public class GameLogic : BehaviourPrimitive
{
    private void OnBegin()
    {
        TransformComponent transform = GetComponent<TransformComponent>();

        SLog.WriteLine("My name: " + GetName());
        SLog.WriteLine("My tag: " + GetTag());

        transform.Position = new Vector3(0, 2, 0);

        var mesh = AddComponent<MeshComponent>();
        mesh.LoadModel("../samples/models/tank.gltf");

        var rb = AddComponent<RigidBodyComponent>();
        var rbInfo = new RigidBodyCreateInfo();
        rbInfo.CreateDefault();
        rbInfo.eType = RigidBodyType.Static;

        rb.Create(rbInfo);

        SLog.WriteLine("OnBegin: exit", LogLevel.Warn);
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