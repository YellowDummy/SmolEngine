using System;

namespace SmolEngine
{
    public class GameLogic : BehaviourPrimitive
    {
        private void OnBegin()
        {
            TransformComponent transform = GetComponent<TransformComponent>();

            SLog.WriteLine(transform.ToString());
        }

        private void OnUpdate()
        {
            if(Input.IsKeyPressed(KeyCode.V))
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
