
#include <xss/object_model.h>
#include <xss/xml_data_reader.h>

using namespace xkp;

//object_model
void object_model::load(DataReader reader, param_list& args)
  {
    //read params
    entity_list parameters = reader->find_all("parameter");
    entity_list::iterator pit = parameters.begin();
    entity_list::iterator pnd = parameters.end();

    for(; pit != pnd; pit++)
      {
        XSSObject xso = *pit;
        str id = xso->get<str>("name", str());
        if (id.empty())
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SNamelessProjectParameter);
            xss_throw(error);
          }

        params_.add(id, xso->get("value", variant()));
      }

    std::vector<XSSObject>::iterator it = applications.begin();
    std::vector<XSSObject>::iterator nd = applications.end();

    for(; it != nd; it++)
      {
        XSSObject app_data = *it;

        //get the appropiate data
        str entry_point = app_data->get<str>("entry_point", str());
        if (entry_point.empty())
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SMustProvideEntryPointForApplicationType);
            xss_throw(error);
          }

        str language_name = app_data->get<str>("language", str());
        if (language_name.empty())
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SMustProvideLanguageForApplicationType);
            xss_throw(error);
          }

        Language lang = get_language(language_name);
        if (!lang)
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SNotaLanguage);
            error.add("language", language_name);
            xss_throw(error);
          }

        fs::path path = fs::complete(base_path_ / entry_point);
        XSSApplicationRenderer app(new xss_application_renderer(path, lang, shared_from_this()));

        //create the "project" object, mainly used to store outside parameters
        XSSObject project(new xss_object);

        for(size_t ii = 0; ii < args.size(); ii++)
          {
            str     pn = args.get_name(ii);
            variant vv = args.get(ii);

            project->add_attribute(pn, vv);
          }

        for(size_t ii = 0; ii < params_.size(); ii++)
          {
            str     pn = params_.get_name(ii);
            variant vv = params_.get(ii);

            project->add_attribute(pn, vv);
          }

        //and register it
        XSSContext ctx = app->context();
        ctx->register_symbol(RESOLVE_CONST, "project", project);

        //and some other crap
        ctx->register_symbol(RESOLVE_CONST, "#app", app);
        register_language_objects(language_name, app->context());

        fs::path op = fs::path(app_data->get<str>("output", str()));
        app->output_path(op);

        options_ = app_data->get<XSSObject>("options", XSSObject());

        //load modules
        std::vector<XSSObject> modules = app_data->find_by_class("idiom");
        std::vector<XSSObject>::iterator it = modules.begin();
        std::vector<XSSObject>::iterator nd = modules.end();

        for(; it != nd; it++)
          {
            XSSObject module = *it;
            str src = module->get<str>("src", str());

            XSSModule compiled = read_module(src, app, module);

            app->register_module(compiled->id(), compiled);
          }

        applications_.push_back(app);
      }

    return project_data;
  }

