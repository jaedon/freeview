module.exports = function(grunt) {
    var fs = require("fs");
    var pathSrc = "src/trunk";
    var prefixWith = (function(prefixPath) {
        if (prefixPath[prefixPath.length - 1] !== "/") {
            prefixPath += "/";
        }
        function prefixWith(pathArray, replacePrefix) {
            return pathArray.map(function(path) {
                var bang = "";
                if (path[0] === "!") {
                    path = path.slice(1);
                    bang = "!";
                }
                return bang + prefixPath + path;
            });
        }
        return prefixWith;
    })(pathSrc);
    try {
        var productName = fs.readFileSync(pathSrc + "/product_name").toString().trim().toLowerCase();
    } catch (e0) {
        console.error("there is no product_name file in " + pathSrc + " directory");
        console.error("cat > " + pathSrc + "/product_name<CR>your_prorduct_name<CTRL-D><CTRL-D>");
        process.exit();
    }
    console.log(productName);
    var dirsOutOfProduct = fs.readdirSync(pathSrc + "/product");
    dirsOutOfProduct.splice(dirsOutOfProduct.indexOf(productName), 1);

    console.log("dist path: " + __dirname + "/dist")
    var config = {
        pkg: grunt.file.readJSON("package.json"),
        uglify: {
            options: {
                preserveComments: false,
                banner: "/*! <%= pkg.name %> <%= grunt.template.today('yyyy-mm-dd') %> */\n",
            },
            dev: {
                cwd: pathSrc,
                expand: true,
                src: ["**/*.js", "!design_result2/**/*.js"],
                dest: "dist"
            },
            release: {
                cwd: pathSrc,
                expand: true,
                src: [
                    "**/*.js",
                    "!emul/**/*.js",
                    "!design_result2/**/*.js",
                    "!snippet/**/*.js",
                    "!test_ts/**/*.js",
                    "!lib/ext/**/*.min.js"
                ],
                dest: "dist"
            }
        },
        concat: {
            options: {
                separator: ";"
            },
            dist: {
                cwd: "src/trunk",
                src: ["**/*.js"],
                dest: "dist/css.js"
            },
            components: {
                src: [pathSrc + "/pages/component/cmpt_*.js"],
                dest: pathSrc + "/pages/component/components_compressed.js"
            }
        },
        cssmin: {
            minify: {
                options: {
                    cwd: "src/trunk",
                    keepSpecialComments: 0
                },
                files: {
                    "dist/css/style.min.css": prefixWith([
                        "css/**/*.css",
                        "prismux/*.css",
                        /*
                         * FIXME: ! must exclude below
                         */
                        "!prismux/prism.dlg.help.css",
                        "!prismux/prism.dlg.hub.css",
                        "!prismux/prism.dlg.image.css"
                    ]),
                    "<%= variables.dist_product_css %>": prefixWith([
                        "product/" + productName + "/css/*.css"
                    ]),
                    "dist/builtin_apps/home_menu.min.css": prefixWith([
                        "prismux/common.css",
                        "prismux/prism.dlg.hub.css"
                    ]),
                    "dist/builtin_apps/global_dialog.min.css": prefixWith([
                        "prismux/common.css",
                        "prismux/prism.dlg.hub.css",
                        "prismux/prism.dlg.image.css",
                        "prismux/prism.dlg.help.css"
                    ])
                }
            }
        },
        copy: {
            product_name: {
                src: pathSrc + "/product_name",
                dest: "dist/product_name"
            },
            license: {
                src: pathSrc + "/HUMAX_LICENSE.txt",
                dest: "dist/HUMAX_LICENSE.txt"
            },
            html_main: {
                expand: true,
                cwd: pathSrc,
                src: "default.html",
                dest: "dist/"
            },
            html_child: {
                expand: true,
                cwd: pathSrc,
                src: "builtin_apps/**/*.html",
                dest: "dist/"
            },
            i18n: {
                expand: true,
                cwd: pathSrc,
                src: "res/*.json",
                dest: "dist/"
            },
            product_data_json: {
                expand: true,
                cwd: pathSrc,
                src: "product/**/*.json",
                dest: "dist/"
            },
            pages_json: {
                expand: true,
                cwd: pathSrc,
                src: "pages/**/*.json",
                dest: "dist/"
            },
            font: {
                expand: true,
                cwd: pathSrc,
                src: "font/**/*",
                dest: "dist/"
            },
            images: {
                expand: true,
                cwd: pathSrc,
                src: ["images/**/*", "product/" + productName + "/images/**/*"],
                dest: "dist/"
            },
            emul: {
                expand: true,
                cwd: pathSrc,
                src: ["emul/**/*", "!emul/**/*.ts"],
                dest: "dist/"
            },
            lib_minified: {
                expand: true,
                cwd: pathSrc,
                src: "lib/ext/**/*.min.js",
                dest: "dist/"
            },
            copy_to_genroot_dev: {
                expand: true,
                cwd: pathSrc,
                src: "**/*",
                dest: process.env.PWD + "/_gen_rootfs/root/usr/browser/webapps"
            },
            copy_to_genroot: {
                expand: true,
                cwd: "dist",
                src: "**/*",
                dest: process.env.PWD + "/_gen_rootfs/root/usr/browser/webapps"
            },
            copy_to_nfsroot: {
                expand: true,
                cwd: "dist",
                src: "**/*",
                dest: "/nfsroot/" + process.env.USER + "/" + productName + "/root/usr/browser/webapps/"
            }
        },
        clean: {
            remove_unrelated_product_files_from_src: dirsOutOfProduct.map(function(path) { return pathSrc + "/product/" + path; }),
            genroot_webapp: [process.env.PWD + "/_gen_rootfs/root/usr/browser/webapps"],
            nfsroot_webapp: ["/nfsroot/" + process.env.USER + "/" + productName + "/root/usr/browser/webapps/*"],
            dist: [__dirname + "/dist"]
        },
        htmlbuild: {
            // added by addToHtmlBuild function
            dev: {
            },
            release: {
            }
        },
        variables: {
            src: pathSrc,
            sugar_main: {
                src: pathSrc + "/default.html",
                dest: pathSrc + "/default.html"
            },
            sugar_home_menu: {
                src: pathSrc + "/builtin_apps/home_menu/home_menu.html",
                dest: pathSrc + "/builtin_apps/home_menu/home_menu.html"
            },
            sugar_global_dialog: {
                src: pathSrc + "/builtin_apps/global_dialog/global_dialog.html",
                dest: pathSrc + "/builtin_apps/global_dialog/global_dialog.html"
            },
            dist_sugar_main: {
                src: "dist/default.html",
                dest: "dist/default.html"
            },
            dist_sugar_home_menu: {
                src: "dist/builtin_apps/home_menu/home_menu.html",
                dest: "dist/builtin_apps/home_menu/home_menu.html"
            },
            dist_sugar_global_dialog: {
                src: "dist/builtin_apps/global_dialog/global_dialog.html",
                dest: "dist/builtin_apps/global_dialog/global_dialog.html"
            },
            dist_product_css: "dist/product/" + productName + "/css/product.min.css"
        },
        "string-replace": {
            patch_html_relative_path: {
                files: {
                    "<%= variables.sugar_main.dest %>": "<%= variables.sugar_main.src %>",
                    "<%= variables.sugar_home_menu.dest %>": "<%= variables.sugar_home_menu.src %>",
                    "<%= variables.sugar_global_dialog.dest %>": "<%= variables.sugar_global_dialog.src %>",
                    "<%= variables.dist_sugar_main.dest %>": "<%= variables.dist_sugar_main.src %>",
                    "<%= variables.dist_sugar_home_menu.dest %>": "<%= variables.dist_sugar_home_menu.src %>",
                    "<%= variables.dist_sugar_global_dialog.dest %>": "<%= variables.dist_sugar_global_dialog.src %>"
                },
                options: {
                    replacements: [{
                        pattern: /<link type="text\/css" rel="stylesheet" href="..\/*/g,
                        replacement: function(match) {
                            return match.substring(0, match.length - 3);
                        }
                    }]
                }
            },
            remove_log_hx: {
                files: {
                    "<%= variables.src %>": pathSrc + "/**.js"
                },
                options: {
                    replacements: [{
                        pattern: /(hx.log[add|remove]*\(.*\);)/g,
                        replacement: function(match) {
                            return "\/\/removed " + match;
                        }
                    }]
                }
            }
        },
        typescript: {
            base: {
                src: [
                    pathSrc + '/**/*.ts',
                    '!' + pathSrc + '/design_result*/**/*.ts',
                    '!' + pathSrc + '/emul/**/*.ts',
                    '!' + pathSrc + '/test/**/*.ts'
                ],
                dest: pathSrc,
                options: {
                    module: 'amd', //or commonjs
                    target: 'es5', //or es3
                    basePath: pathSrc,
                    declaration: true
                }
            }
        },
        less: {
            generate_css: {
                path: ["css", "prismux", "prismux/controls", "product/" + productName + "/css"],
                files: [
                    {
                        expand: true,
                        cwd: pathSrc,
                        src: ["**/*.less"],
                        dest: pathSrc,
                        ext: ".css"
                    }
                ]
            }
        },
        watch: {
//            css: {
//                files: [ pathSrc + "/**/*.css" ],
//                task:
//            }
        }
    };

    var addToHtmlBuild = function(aMode, aConfig, aKey, aSrc, aDest, aCss, aSection, aData) {
        if (aMode !== "dev" && aMode !== "release") {
            throw "aMode(" + aMode + ") must dev or release";
        }
        if (typeof aConfig !== "object") {
            throw "aConfig(" + aConfig + ") must string";
        }
        if (typeof aKey !== "string") {
            throw "aKey(" + aKey + ") must string";
        }
        if (typeof aSrc !== "string") {
            throw "aSrc(" + aSrc + ") must string";
        }
        if (typeof aDest !== "string") {
            throw "aDest(" + aDest + ") must string";
        }
        if (!Array.isArray(aCss)) {
            throw "aCss(" + aCss + ") must array";
        }

        var item = {
            src: aSrc,
            dest: aDest,
            options: {
                relative: true,
                styles: {
                    bundle: aCss
                }
            }
        };
        if (aSection) {
            if (typeof aSection !== "string" && typeof aSection !== "object") {
                throw aSection + " aSection(" + aSection + ") must string or object";
            }
            item.options.sections = aSection;
        }
        if (aData) {
            if (typeof aData !== "object") {
                throw "aSection(" + aData + ") must object";
            }
            item.options.data = aData;
        }
        console.log("addToHtmlBuild: " + aKey + " to " + "aConfig.htmlbuild." + aMode + "." + aKey);
        console.log("addToHtmlBuild: " + "src: " + aSrc + " aDest: " + aDest);
        console.log("addToHtmlBuild: " + aCss.join("\n"));
        //aConfig.htmlbuild[aMode][aKey] = item;
        aConfig.htmlbuild[aKey] = item;
    };
    addToHtmlBuild(
        "dev",
        config,
        "sugar_main",
        "template_html/template_sugar_main.html",
        pathSrc + "/default.html",
        [
            pathSrc + "/css/**/*.css",
            pathSrc + "/prismux/**/*.css",
            "!" + pathSrc + "/prismux/prism.dlg.help.css",
            "!" + pathSrc + "/prismux/prism.dlg.hub.css",
            "!" + pathSrc + "/prismux/prism.dlg.image.css",
            pathSrc + "/product/hdr4000t/css/*.css",

        ],
        { layout: "template_html/html_body_sugar.html" },
        {
            require: JSON.stringify({
                baseUrl: "./",
                waitSeconds: 0
            })
        }
    );
    addToHtmlBuild(
        "dev",
        config,
        "sugar_home_menu",
        "template_html/template_sugar_child.html",
        pathSrc + "/builtin_apps/home_menu/home_menu.html",
        [
            pathSrc + "/prismux/common.css",
            pathSrc + "/prismux/prism.dlg.hub.css",
            pathSrc + "/product/hdr4000t/css/font_helvetica.css"

        ],
        null,
        {
            require: JSON.stringify({
                baseUrl: "../../",
                waitSeconds: 0
            }),
            requestInfo: JSON.stringify({
                ansi: "[1;32;41m",
                deps: [
                    "prismux/prism.dlg",
                    "prismux/prism.dlg.button",
                    "prismux/prism.dlg.hub",
                    "builtin_apps/builtin_common"
                ],
                url: ["builtin_apps/home_menu/home_menu"]
            })
        }
    );
    addToHtmlBuild(
        "dev",
        config,
        "sugar_global_dialog",
        "template_html/template_sugar_child.html",
        pathSrc + "/builtin_apps/global_dialog/global_dialog.html",
        [
            pathSrc + "/prismux/common.css",
            pathSrc + "/prismux/prism.dlg.hub.css",
            pathSrc + "/prismux/prism.dlg.image.css",
            pathSrc + "/prismux/prism.dlg.help.css",
            pathSrc + "/product/hdr4000t/css/font_helvetica.css"
        ],
        null,
        {
            require: JSON.stringify({
                baseUrl: "../../",
                waitSeconds: 0
            }),
            requestInfo: JSON.stringify({
                ansi: "[1;35;47m",
                deps: [
                    "prismux/prism.dlg",
                    "prismux/prism.dlg.notification",
                    "prismux/prism.dlg.selection",
                    "prismux/prism.dlg.button",
                    "prismux/prism.dlg.image",
                    "prismux/prism.dlg.help",
                    "prismux/prism.dlg.hub",
                    "builtin_apps/builtin_common"
                ],
                url: ["builtin_apps/global_dialog/global_dialog"]
            })
        }
    );
    /*
     * to release
     */
    addToHtmlBuild(
        "release",
        config,
        "dist_sugar_main",
        "template_html/template_sugar_main.html",
        "dist/default.html",
        [
            "dist/css/**/*.css",
            "dist/product/" + productName + "/css/*.css"
        ],
        { layout: "template_html/html_body_sugar.html" },
        {
            require: JSON.stringify({
                baseUrl: "./",
                waitSeconds: 0
            })
        }
    );
    addToHtmlBuild(
        "release",
        config,
        "dist_sugar_home_menu",
        "template_html/template_sugar_child.html",
        "dist/builtin_apps/home_menu/home_menu.html",
        [
            "dist/builtin_apps/home_menu.min.css",
            "dist/product/" + productName + "/css/*.css"
        ],
        null,
        {
            require: JSON.stringify({
                baseUrl: "../../",
                waitSeconds: 0
            }),
            requestInfo: JSON.stringify({
                ansi: "[1;32;41m",
                deps: [
                    "prismux/prism.dlg",
                    "prismux/prism.dlg.button",
                    "prismux/prism.dlg.hub",
                    "builtin_apps/builtin_common"
                ],
                url: ["builtin_apps/home_menu/home_menu"]
            })
        }
    );
    addToHtmlBuild(
        "release",
        config,
        "dist_sugar_global_dialog",
        "template_html/template_sugar_child.html",
        "dist/builtin_apps/global_dialog/global_dialog.html",
        [
            "dist/builtin_apps/home_menu.min.css",
            "dist/product/" + productName + "/css/*.css"
        ],
        null,
        {
            require: JSON.stringify({
                baseUrl: "../../",
                waitSeconds: 0
            }),
            requestInfo: JSON.stringify({
                ansi: "[1;35;47m",
                deps: [
                    "prismux/prism.dlg",
                    "prismux/prism.dlg.notification",
                    "prismux/prism.dlg.selection",
                    "prismux/prism.dlg.button",
                    "prismux/prism.dlg.image",
                    "prismux/prism.dlg.help",
                    "prismux/prism.dlg.hub",
                    "builtin_apps/builtin_common"
                ],
                url: ["builtin_apps/global_dialog/global_dialog"]
            })
        }
    );


    grunt.initConfig(config);

    grunt.loadNpmTasks("grunt-contrib-uglify");
    grunt.loadNpmTasks("grunt-contrib-concat");
    grunt.loadNpmTasks("grunt-contrib-cssmin");
    grunt.loadNpmTasks("grunt-contrib-copy");
    grunt.loadNpmTasks("grunt-contrib-clean");
    grunt.loadNpmTasks("grunt-html-build");
    grunt.loadNpmTasks("grunt-string-replace");
    grunt.loadNpmTasks("grunt-typescript");
    grunt.loadNpmTasks("grunt-contrib-watch");
    grunt.loadNpmTasks("grunt-contrib-less");

    grunt.registerTask("default", [
//        "clean:unrelative_product_remove_from_src",
        "copy:product_name",
        "copy:license",
        "htmlbuild",
        "string-replace:patch_html_relative_path"
    ]);
    grunt.registerTask("make_skelenton", [
        "clean"
    ]);
    grunt.registerTask("copy_to_nfsroot", [
        "clean:nfsroot_webapp",
        "copy"]
    );
    grunt.registerTask("test", [
        "clean:remove_unrelated_product_files_from_src",
        "cssmin",
        "copy:product_name",
        "copy:license",
        "htmlbuild:dev"]
    );
    
    //used by Makefile
    grunt.registerTask("debug_by_makefile", [
        "uglify:dev",
        "htmlbuild",
        "string-replace:patch_html_relative_path",
        "copy:product_name",
        "copy:license",
        "copy:i18n",
        "copy:product_data_json",
       	"copy:pages_json",
       	"copy:lib_minified",
       	"copy:images",
       	"copy:font",
       	"copy:emul",
       	"copy:copy_to_genroot_dev"
    ]);
    grunt.registerTask("release_by_makefile", [
        "clean",
        "uglify:release",
        "cssmin",
        "htmlbuild",
        "string-replace:patch_html_relative_path",
        "copy:product_name",
        "copy:license",
        "copy:i18n",
        "copy:product_data_json",
        "copy:pages_json",
        "copy:lib_minified",
        "copy:images",
        "copy:font",
        "copy:copy_to_genroot"
    ]);

    //just webapp release
    grunt.registerTask("release", [
        "clean:remove_unrelated_product_files_from_src",
        "uglify:release",
        "cssmin",
        "htmlbuild",
        "string-replace:patch_html_relative_path",
        "copy:product_name",
        "copy:license",
        "copy:i18n",
        "copy:product_data_json",
        "copy:pages_json",
        "copy:lib_minified",
        "copy:images",
        "copy:font",
        "copy:emul"]
    );
};
