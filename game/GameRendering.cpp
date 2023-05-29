// Example of a simple rendering function
void RenderObject(GameObject object) {
    // Apply object transformations
    // Set up rendering state (e.g., texture, shader)
    // Render the object's geometry
}

// Example usage in the game loop
GameEngine gameEngine;
gameEngine.Initialize();

while (gameIsRunning) {
    gameEngine.Update();
    gameEngine.Render();
}
