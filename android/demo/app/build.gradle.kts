plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "net.parabola2004.spargeldemo"
    compileSdkVersion = "android-34"

    defaultConfig {
        applicationId = "net.parabola2004.spargeldemo"
        minSdk = 30
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        externalNativeBuild {
            cmake {
                targets("simple_entry_demo")
                arguments(
                    "-DANDROID_STL=c++_shared",
                    "-DSPARGEL_ENABLE_SANITIZER_ADDRESS=OFF",
                    "-DSPARGEL_GPU_ENABLE_VULKAN=OFF"
                )
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    kotlinOptions {
        jvmTarget = "11"
    }

    buildFeatures {
        prefab = true
    }

    externalNativeBuild {
        cmake {
            path = file("../../../CMakeLists.txt")
            version = "3.31.1"
        }
    }
}

dependencies {
    implementation(libs.androidx.appcompat)
    implementation(libs.androidx.games.activity)
}