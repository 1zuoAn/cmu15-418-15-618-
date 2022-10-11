package Class;

import java.sql.*;
import java.util.ArrayList;

/**
 * @author 林子键
 * @version 1.0
 */
public class Read {

    private Connection con = null;
    ;
    //存储用户名和密码信息
    private String username = null;
    private String password = null;

    public String getUsername() {
        return username;
    }

    public String getPassword() {
        return password;
    }

    public Connection getCon() {
        return con;
    }

    public Connection createConnector(String username, String password) {
        Connection conn = null;
        //数据库连接驱动名：针对不同的数据库，驱动名称不同，但是同一种类型的数据库改字符串相同
        String driver = "com.mysql.cj.jdbc.Driver";
        //URL指向要访问的数据库study,useSSL用来指明是否需要使用SSL协议，如果不指明将会报错
        String url = "jdbc:mysql://localhost:3306/point?useSSL=true";
        try {
            //加载驱动程序
            Class.forName(driver);
            //连接数据库
            conn = DriverManager.getConnection(url, username, password);
            //检查数据库连接是否成功
            if (!conn.isClosed()) {
                System.out.println("Successed connecting to the Database!");
                //存储当前连接的用户的用户名和密码
                this.username = username;
                this.password = password;
                this.con = conn;
            }

        } catch (ClassNotFoundException e) {
            System.out.println("Sorry, can't find the Driver!");
        } catch (SQLException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return conn;
    }

    //读入学生信息(仅仅是点名方可见的基础信息)，读入课程信息以及一轮的真实学生出勤情况(点名方不可见)
    public Lessons readDB(Lessons lessons, int cnt) {

        try {
            ResultSet rs = query("SELECT * FROM RECORDS;");
            ArrayList<Student> students1 = new ArrayList<Student>();
            ArrayList<Student> students2 = new ArrayList<Student>();
            ArrayList<Student> students3 = new ArrayList<Student>();
            ArrayList<Student> students4 = new ArrayList<Student>();
            ArrayList<Student> students5 = new ArrayList<Student>();
            Lessons lessons1 = new Lessons();
            Course course1 = new Course(lessons.getCourses().get(0).getName(), 20);
            Course course2 = new Course(lessons.getCourses().get(1).getName(), 20);
            Course course3 = new Course(lessons.getCourses().get(2).getName(), 20);
            Course course4 = new Course(lessons.getCourses().get(3).getName(), 20);
            Course course5 = new Course(lessons.getCourses().get(4).getName(), 20);
            int i1 = 0;
            int i2 = 0;
            int i3 = 0;
            int i4 = 0;
            int i5 = 0;
            while (rs.next()) {
                Student student;
                String No = rs.getString("No");
                String str = rs.getString("Course");
                String courseName = str.substring(0, 2);
                student = new Student(No, rs.getString("Name")
                        , rs.getDouble("Grade"));
                if (cnt == 1) {
                    student.buildMis_rate(); //生成初始逃课率
                }
                if (courseName.equals(lessons.getCourses().get(0).getName())) {
                    if (cnt != 1) {
                        student.setMis_rate(lessons.getCourses().get(0).getStudents().get(i1++).getMis_rate());
                        lessons.getCourses().get(0).putRecords(student,changeRecords(str));
                    }else{
                        students1.add(student);
                        course1.putRecords(student, changeRecords(str));
                    }
                } else if (courseName.equals(lessons.getCourses().get(1).getName())) {
                    if (cnt != 1) {
                        student.setMis_rate(lessons.getCourses().get(1).getStudents().get(i2++).getMis_rate());
                        lessons.getCourses().get(1).putRecords(student,changeRecords(str));
                    }else{
                        students2.add(student);
                        course2.putRecords(student, changeRecords(str));
                    }
                } else if (courseName.equals(lessons.getCourses().get(2).getName())) {
                    if (cnt != 1) {
                        student.setMis_rate(lessons.getCourses().get(2).getStudents().get(i3++).getMis_rate());
                        lessons.getCourses().get(2).putRecords(student,changeRecords(str));
                    }else{
                        students3.add(student);
                        course3.putRecords(student, changeRecords(str));
                    }
                } else if (courseName.equals(lessons.getCourses().get(3).getName())) {
                    if (cnt != 1) {
                        student.setMis_rate(lessons.getCourses().get(3).getStudents().get(i4++).getMis_rate());
                        lessons.getCourses().get(3).putRecords(student,changeRecords(str));
                    }else{
                        students4.add(student);
                        course4.putRecords(student, changeRecords(str));
                    }
                } else {
                    if (cnt != 1) {
                        student.setMis_rate(lessons.getCourses().get(4).getStudents().get(i5++).getMis_rate());
                        lessons.getCourses().get(4).putRecords(student,changeRecords(str));
                    }else{
                        students5.add(student);
                        course5.putRecords(student, changeRecords(str));
                    }
                }

            }
            if(cnt == 1) {
                course1.addStudents(students1);
                course2.addStudents(students2);
                course3.addStudents(students3);
                course4.addStudents(students4);
                course5.addStudents(students5);
                lessons1.addLesson(course1);
                lessons1.addLesson(course2);
                lessons1.addLesson(course3);
                lessons1.addLesson(course4);
                lessons1.addLesson(course5);
            }else {
                lessons1 = lessons;
            }
            return lessons1;
        } catch (
                Exception e) {
            System.out.println("false");
        }
        System.out.println("ddd");
        return null;
    }


    public ArrayList<Integer> changeRecords(String str) {
        ArrayList<Integer> integers = new ArrayList<>();
        for (int i = 4; i < str.length(); ++i) {
            integers.add((int) str.charAt(i) - (int) '0');
        }
        return integers;
    }


    public boolean execute(Connection conn, String command) {
        Statement statement;
        try {
            statement = conn.createStatement();
            String sql = command;
            /**
             * Statement接口中的excuteUpdate()方法执行给定的
             * SQL语句，该语句可以是INSERT,UPDATE,DELETE语句
             */
            statement.executeUpdate(sql);
            return true;
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return false;
    }

    public ResultSet query(String query1) {
        //创建Statement对象，用来执行SQL语句
        Statement statement;
        try {
            statement = con.createStatement();
            //需要执行的数据库操作语句
            String sql = query1;
            //执行数据库操作语句并返回结果
            return statement.executeQuery(sql);
        } catch (SQLException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return null;
    }


}
