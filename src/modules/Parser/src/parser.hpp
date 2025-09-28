#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QPoint>
#include <QColor>
#include <fstream>

/**
 * @class Parser
 * @brief A class to parse and execute commands lines/scripts for controlling a Turtle object.
 * 
 * The Parser class has two key functions for parsing single command lines or entire scripts 
 * To execute commands, a corresponding Qt signal is sent to the Turtle module.
 */
class Parser : public QObject {
  Q_OBJECT

private:
    /// @brief Map of variable names to their float values.
    std::unordered_map<std::string, float> vars;
    
    /// @brief Map of function names to their body definitions.
    std::unordered_map<std::string, std::string> funcs;
    
    /// @brief Map of function names to the argument names used as parameters to the function.
    std::unordered_map<std::string, std::string> func_arg_names; 
  
    /// @brief Tracks whether the animation of turtle movement is complete.
    bool movement_done;

    /**
     * @brief Executes a loop and returns the parsed commands.
     * 
     * @param count Number of loop iterations.
     * @param loop_body The body of the loop to execute.
     * @return Vector of commands that were successfully parsed and executed.
     */
    std::vector<std::string> run_loop(int count, const std::string& loop_body); 

    /**
     * @brief Replaces variable names in a command with their respective values.
     * 
     * @param command The command string containing variable names.
     * @return The command string with variables substituted by their values.
     */ 
    std::string varname_to_value(const std::string& command);

 public:
    /**
     * @brief Constructs a Parser object.
     * 
     * @param parent Optional QObject parent.
     */
    explicit Parser(QObject *parent = nullptr);
    
    /**
     * @brief Parses a single line of input and sends the commands to the Turtle.
     * 
     * @param inputQ The input line as a QString.
     * @return Vector of commands that were successfully parsed and executed.
     */
    Q_INVOKABLE std::vector<std::string> parse_line(const QString& inputQ);

    /**
     * @brief Parses an entire script file and executes the commands.
     * 
     * @param file The input script file as an ifstream.
     * @return Vector of commands that were successfully parsed and executed.
     */    
    Q_INVOKABLE std::vector<std::string> parse_script(std::ifstream& file);
 
    signals: // Commands that are sent to the Turtle as signals
      /**
       * @brief Signal to move the turtle forward.
       * @param distance The distance to move forward.
       */        
      void forward(float distance);
      
      /**
       * @brief Signal to turn the turtle by an angle.
       * @param angle The angle to turn in degrees.
       */
      void turn(float angle);
      
      /**
       * @brief Signal to lift the turtle's pen to stop drawing
       */
      void up();
      
      /**
       * @brief Signal to lower the turtle's pen to continue drawing
       */
      void down();

      /**
       * @brief Signal to set the turtle's position.
       * @param pos The new position as a QPointF.
       */
      void setpos(QPointF pos);
      
      /**
       * @brief Signal to set the turtle's rotation.
       * @param rot The new rotation angle in degrees.
       */
      void setrot(float rot);
      
      /**
       * @brief Signal to set the turtle's pen size.
       * @param size The new pen size.
       */
      void setsize(float size);
      
      /**
       * @brief Signal to set the turtle's movement speed.
       * @param speed The new movement speed.
       */
      void setspeed(float speed);
      
      /**
       * @brief Signal to set the color of the turtle.
       * @param color The new color.
       */
      void setcolor(QColor color);

      /**
       * @brief Signal to draw an arc.
       * @param radius The radius of the arc.
       * @param angle The angle to sweep in degrees.
       */
      void arc(float radius, float angle);

  public slots:
    /**
     * @brief Slot to process the end of an animation.
     */
    void animation_done();
};

#endif  // PARSER_H