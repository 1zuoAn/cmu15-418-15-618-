package Class;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Objects;

/**
 * @author 林子键
 * @version 1.0
 */
public class Sample {


    Lessons lessons = new Lessons();

    Lessons pointLessons = new Lessons();

    public Lessons getLessons() {
        return lessons;
    }

    public void setLessons(Lessons lessons) {
        this.lessons = lessons;
    }

    public Lessons getPointLessons() {
        return pointLessons;
    }

    public void setPointLessons(Lessons pointLessons) {
        this.pointLessons = pointLessons;
    }


    public void sampleAllPoint(Lessons lessons,Lessons pointLessons, int cnt){
        ArrayList<Course> courses = lessons.getCourses();
        ArrayList<Course> pointCourses = pointLessons.getCourses();
        ArrayList<Course> lessons1 = new ArrayList<>();
        ArrayList<Course> pointLessons1 = new ArrayList<>();
        for (int i = 0; i < courses.size(); i++) {
            Course course = courses.get(i);
            Course pointCourse = pointCourses.get(i);
            Course course1;
            if (cnt == 1) {
                course1 = pointCourse.pointAllBadStudents(course,cnt);
            } else {
                course1 = pointCourse.pointBadStudent(course,cnt);
            }
            lessons1.add(pointCourse.pointGoodStudent(course1,cnt));
            pointLessons1.add(pointCourse);
        }
        setLessons(new Lessons(lessons1));
        setPointLessons(new Lessons(pointLessons1));
    }


}
